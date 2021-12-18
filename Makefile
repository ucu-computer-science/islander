# Build all islander-related projects.
.PHONY: all islander_engine ps
all: islander_engine ps

# Build 'islander namespaces' project.
islander_engine:
	cmake ./isle -B build/islander_engine && \
	cmake --build build/islander_engine

# Build 'islander ps' project.
ps:
	cmake ./ps -B build/ps && \
	cmake --build build/ps

# Unzip the rootfs directory and place it to where it should be.
install_rootfs:
	mkdir ./isle/ubuntu-rootfs
	tar -xvzf ./isle/files/ubuntu-rootfs.tar.gz -C ./isle/ubuntu-rootfs
	mkdir ./isle/ubuntu-rootfs/test_mnt
	mkdir ./isle/ubuntu-rootfs/test_tmpfs
	mkdir ./isle/ubuntu-rootfs/host_dev
	
# Create all required dirs.
create_dirs:
	mkdir ./isle/islenodes
