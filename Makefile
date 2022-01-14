# Build all islander-related projects.
.PHONY: islander-engine ps delete logger islander-client islander-server
#all: islander-engine ps delete logger islander-client islander-server

# Build all sub-projects
build: islander-engine ps delete logger islander-client islander-server

# Create all necessary dirs and install rootfs
configure: create-home-dir install-rootfs

# Build 'islander namespaces' project.
islander-engine:
	cmake ./isle -B ./isle/build && \
	cmake --build ./isle/build

# Build 'islander ps' project.
ps:
	cmake ./ps -B ./ps/build && \
	cmake --build ./ps/build


delete:
	cmake ./delete -B ./delete/build && \
    cmake --build ./delete/build


logger:
	cmake ./logger_server -B ./logger_server/build && \
	cmake --build ./logger_server/build


islander-client:
	cmake ./islander-client -B ./islander-client/build && \
	cmake --build ./islander-client/build


islander-server:
	cmake ./islander-server -B ./islander-server/build && \
    cmake --build ./islander-server/build

# Unzip the rootfs directory and place it to where it should be.
install-rootfs:
	mkdir -p ./isle/ubuntu-rootfs
	tar -xvzf ./isle/files/ubuntu-rootfs.tar.gz -C ./isle/ubuntu-rootfs
	mkdir -p ./isle/ubuntu-rootfs/test_mnt
	mkdir -p ./isle/ubuntu-rootfs/test_tmpfs
	mkdir -p ./isle/ubuntu-rootfs/host_dev


create-home-dir:
	mkdir -p ~/islander
	mkdir -p ~/islander/islenodes
	mkdir -p ~/islander/logger
	mkdir -p ~/islander/volumes