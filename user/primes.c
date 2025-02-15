#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Hàm này sẽ tạo pipe và write vào pipe từ số 2 đến 279 và trả về p[0] (read) của pipe
int generateNumbers() {
	int p[2];
	
	pipe(p);

	if (fork() == 0) {
		for (int i = 2; i < 280; i++) {
			write(p[1], &i, sizeof(int));
		}
		close(p[1]);

		exit(0);
	}

	close(p[1]);

	return p[0]; // đây là p[0] của tiến trình cha nha
}

// Hàm này sẽ đọc từ in_fd và lọc ra các số không chia hết cho prime và trả về p[0] (read) của pipe
// Hàm này sẽ sẻ dụng descriptor của pipe cũ từ hàm generateNumbers và lọc rồi pass vào pipe mới khác
int filterPrime(int in_fd, int prime) 
{
	int num;
	int p[2];

	pipe(p);

	if (fork() == 0) {
		while (read(in_fd, &num, sizeof(int))) {
			if (num % prime != 0) {
				write(p[1], &num, sizeof(int));
			}
		}
		close(in_fd);
		close(p[1]);
		
		exit(0);
	}

	close(in_fd);
	close(p[1]);

	return p[0];
}

int main(int argc, char *argv[])
{
	int prime;
	
	int i = generateNumbers();

	// Bản chất là đọc số đầu tiên từ pipe, in ra
	while (read(i, &prime, sizeof(int))) {
		printf("prime %d\n", prime); 
		i = filterPrime(i, prime);
	}

	exit(0);
}

/*
Mô tả quá trình:

 2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 ...
 ↑
 Đọc số đầu tiên -> prime 2 (In ra màn hình)
 Lọc số chia hết cho 2:
  3  5  7  9 11 13 15 17 ...
    ↑
    Đọc số đầu tiên -> prime 3 (In ra màn hình)
    Lọc số chia hết cho 3:
     5  7 11 13 17 ...
       ↑
       Đọc số đầu tiên -> prime 5 (In ra màn hình)
       Lọc số chia hết cho 5:
         7 11 13 17 ...
           ↑
           Đọc số đầu tiên -> prime 7 (In ra màn hình)
           ...

*/