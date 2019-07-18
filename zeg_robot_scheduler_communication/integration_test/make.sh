g++ -std=c++17 -g -o zeg_robot_simulator zeg_robot_simulator.cpp -I ../src -I ../include/rest_rpc -I ../include/msgpack -I /opt/boost_1_69_0 -pthread
g++ -std=c++17 -g -o zeg_robot_send_task zeg_robot_send_task.cpp -I ../src -I ../include/rest_rpc -I ../include/msgpack -I /opt/boost_1_69_0 -pthread
g++ -std=c++17 -g -o zeg_robot_tcs zeg_robot_tcs.cpp -I ../src -I ../include/rest_rpc -I ../include/msgpack -I /opt/boost_1_69_0 -pthread
mv -f zeg_robot_tcs zeg_robot_send_task zeg_robot_simulator ../bin/
