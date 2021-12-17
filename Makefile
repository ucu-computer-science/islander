# Build all islander-related projects.
.PHONY: all islander_enjine ps
all: islander_enjine ps

# Build 'islander namespaces' project.
islander_enjine:
	cmake ./isle -B build/islander_enjine && \
	cmake --build build/islander_enjine

# Build 'islander ps' project.
ps:
	cmake ./ps -B build/ps && \
	cmake --build build/ps

# Unzip the rootfs directory and place it to where it should be.
install_rootfs:
	mkdir ./isle/ubuntu-rootfs
	tar -xvzf ./isle/files/ubuntu-rootfs.tar.gz -C ./isle/ubuntu-rootfs
