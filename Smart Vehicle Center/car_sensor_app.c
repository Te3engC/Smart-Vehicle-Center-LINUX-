#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

struct __attribute__((packed)) sensor_packet {
    int distance;
    int temperature;
    int humidity;
    unsigned long ts;
};

static int running = 1;
void stop(int sig) { running = 0; }

int main() {
    int fd = open("/dev/car_sensor", O_RDONLY);
    struct sensor_packet p;
    signal(SIGINT, stop);

    if (fd < 0) { perror("Open failed"); return -1; }

    while (running) {
        if (read(fd, &p, sizeof(p)) == sizeof(p)) {
            printf("Dist: %3dcm | Temp: %2dC | Hum: %2d%% | TS: %lu\n", 
                    p.distance, p.temperature, p.humidity, p.ts);
        } else {
            printf("Read error or interrupted\n");
            break;
        }
        sleep(2); // ±Ü¿ª²ÉÑù³åÍ»
    }
    close(fd);
    return 0;
}