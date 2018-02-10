program PUBSUBGROUP {
   version PUBSUBGROUP_VER {
     int JOIN(string, int) = 1;
     int LEAVE(string, int) = 2;
     int SUBSCRIBE(string, int, string) = 3;
     int UNSUBSCRIBE(string, int, string) = 4;
     int PUBLISH(string, string, int) = 5;
     int PING() = 6;
   } = 1;
} = 0x20000001;
