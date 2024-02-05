#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// 인터럽트 핸들러 함수
void interrupt_handler(int signum) {
    printf("Interrupt signal received!\n");
    // 실제 인터럽트 핸들링 코드를 여기에 작성합니다.
}

int main() {
    // SIGINT(일반적으로 Ctrl+C로 발생시키는 시그널)에 대한 핸들러 설정
    signal(SIGINT, interrupt_handler);

    while(1) {
        printf("Running...\n");
        sleep(1); // 1초 동안 프로그램 실행을 잠시 멈춥니다.
    }

    return 0;
}

