//
// Created by yaroslav_morozevych on 16.10.21.
//
#ifndef NAMESPACES_MNTNAMESPACE_H
#define NAMESPACES_MNTNAMESPACE_H

// File / Directory Permissions
#define ALL_PERMISSIONS 0777         // -rwxrwxrwx
#define READ_N_EXEC_PERMISSIONS 0555 // -r-xr-xr-x

// The ext4 (Fourth Extended FS) is Linux file system that has historically been the default
// file system for many Linux distributions. It is general purpose file system that
// has been designed for extensibility and backwards compatibility.
#define EXT4 "ext4"


/* Configure /proc file system by mounting it in
 * current MOUNT and PID namespaces. */
static void configure_procfs() {
    // Create "/proc" directory with certain permissions.
    if (mkdir("/proc", READ_N_EXEC_PERMISSIONS) && errno != EEXIST) kill_process("Failed to mkdir /proc: %m\n");
    // Mount "/proc" in current MNT and PID namespaces.
    if (mount("proc", "/proc", "proc", 0, "")) kill_process("Failed to mount proc: %m\n");
}


/* Set up the current MOUNT namespace by pivoting the root fs of the process to "rootfs".
 * rootfs: Path to the directory containing system files (e.g. Alpine Dist. FS) */
static void setup_mntns(char *rootfs) {
    // Path to the mounting point.
    const char *mnt = rootfs;

    // Create a bind (MS_BIND) mount of "rootfs" as source to "rootfs" target.
    if (mount(rootfs, mnt, EXT4, MS_BIND, "")) kill_process("Failed to mount %s at %s: %m\n", rootfs, mnt);

    // Change the current directory to "rootfs".
    if (chdir(mnt)) kill_process("Failed to chdir to rootfs mounted at %s: %m\n", mnt);

    // Create folder 'put_old' for the old root file system.
    const char *put_old = ".put_old";
    if (mkdir(put_old, ALL_PERMISSIONS) && errno != EEXIST) kill_process("Failed to mkdir put_old %s: %m\n", put_old);

    // Use PIVOT_ROOT Syscall to mount "." (rootfs) as a new Root file system and to make "put_old" contain the old root.
    if (syscall(SYS_pivot_root, ".", put_old)) kill_process("Failed to pivot_root from %s to %s: %m\n", rootfs, put_old);

    // Change the current directory to "/" -> Root.
    if (chdir("/")) kill_process("Failed to chdir to new root: %m\n");

    // Configure "/proc" fs.
    configure_procfs();

    // Unmount the old root fs.
    if (umount2(put_old, MNT_DETACH)) kill_process("Failed to umount put_old %s: %m\n", put_old);
}


#endif //NAMESPACES_MNTNAMESPACE_H
