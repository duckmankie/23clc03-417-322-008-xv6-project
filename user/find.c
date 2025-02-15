#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

// Hàm kiểm tra xem tên file hiện tại có trùng với targetname không
int is_match(const char *path, const char *targetname) {
    char *p = (char *)path + strlen(path);  // Tìm vị trí cuối cùng của path

    // Lùi về ký tự '/' gần nhất
    while (p >= path && *p != '/') p--;
    p++;// Nhảy qua dấu '/'

    // So sánh với targetname
    return strcmp(p, targetname) == 0;
}

// Hàm tìm file đệ quy trong thư mục
void find(const char *path, const char *targetname) {
    char buffer[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // Nếu file hiện tại có tên trùng targetname, in ra màn hình
    if (is_match(path, targetname)) {
        printf("%s\n", path);
    }

    // Mở thư mục
    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // Lấy thông tin file/thư mục
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // Nếu không phải thư mục, thoát
    if (st.type != T_DIR) {
        close(fd);
        return;
    }

    // Kiểm tra độ dài đường dẫn trước khi nối thêm tên file
    if (strlen(path) + 1 + DIRSIZ >= sizeof(buffer)) {
        printf("find: path too long\n");
        close(fd);
        return;
    }

    // Sao chép đường dẫn thư mục vào buffer
    strcpy(buffer, path);
    p = buffer + strlen(buffer);
    *p++ = '/';

    // Đọc từng file trong thư mục
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) continue;  // Bỏ qua file rỗng

        // Bỏ qua "." và ".." để tránh vòng lặp vô hạn
        if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
            continue;

        // Nối tên file vào đường dẫn
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        // Gọi đệ quy để tìm tiếp
        find(buffer, targetname);
    }

    close(fd);  // Đóng thư mục sau khi duyệt xong
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(2, "usage: find path filename\n");
        exit(1);
    }

    find(argv[1], argv[2]);  // Gọi hàm find với đường dẫn và tên file cần tìm

    exit(0);
}
