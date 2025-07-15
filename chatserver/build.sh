echo "Building Chat Server..."
gcc chatserver.c ../chatsock.c ../chatsock_list.c -o chatserver
echo "Running Chat Server..."
./chatserver
