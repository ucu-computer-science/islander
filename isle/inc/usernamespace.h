#ifndef NAMESPACES_USERNAMESPACE_H
#define NAMESPACES_USERNAMESPACE_H

#define ROOT_UID 0
#define ROOT_GID 0
#define UID 1000


/* Map the user privillages of the parent process to the root of the child one. */
static void set_userns_mappings(int pid) {
    char path[100];
    char line[100];

    int uid = UID;

    sprintf(path, "/proc/%d/uid_map", pid);
    sprintf(line, "0 %d 1\n", uid);
    write_file(path, line);

    sprintf(path, "/proc/%d/setgroups", pid);
    sprintf(line, "deny");
    write_file(path, line);

    sprintf(path, "/proc/%d/gid_map", pid);
    sprintf(line, "0 %d 1\n", uid);
    write_file(path, line);
}


/* Give the ROOT privillages to the child process by setting
 * UID and GID to 0 (root) */
static int set_userns_ids() {
    // Assuming, 0 in the current namespace maps to
    // a non-privileged UID in the parent namespace,
    // drop superuser privileges if any by enforcing
    // the exec'ed process runs with UID 0.
    if (setgid(ROOT_GID) == -1) kill_process("Failed to setgid: %m\n");
    if (setuid(ROOT_UID) == -1) kill_process("Failed to setuid: %m\n");
}

#endif //NAMESPACES_USERNAMESPACE_H
