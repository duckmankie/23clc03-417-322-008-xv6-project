#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(){ // từ lúc run main thì đây là 1 tiến trình cha

    // tạo pipe
    int p[2];
    pipe(p);
    // mô tả : [tiến trình 1] ---> write() ---> (Pipe) ---> read() ---> [tiến trình 2]
    // p[0] là đầu đọc (read)
    // p[1] là đầu ghi (write)
    
    // tạo ra tiến trình con
    int pid = fork(); 
    
    // pid = 0 -> đang ở trong tiến trình con
    if (pid == 0) 
    {
        // tạo buffer lưu trữ dữ liệu, trong ví dụ này chỉ gửi 1 byte
        char buffer[1];
        read(p[0], &buffer, 1); // nhận dữ liệu từ cha
        printf("%d: received ping\n", getpid()); // getpid() -> trả về ID của tiến trình hiện tại
        write(p[1], &buffer, 1); // gửi lại cho cha
    }
    else // pid > 0 -> đang ở trong tiến trình cha
    {
        char message = 'a';
        write(p[1], &message, 1);
        wait(0); // nếu không có wait(0), tiến trình cha có thể đọc dữ liệu từ pipe quá sớm trước khi tiến trình con kịp gửi lại
        read(p[0], &message, 1);
        printf("%d: received pong\n", getpid());
    }
    exit(0);
}