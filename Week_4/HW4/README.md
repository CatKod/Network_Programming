# IT4062 - Homework 4: TCP socket applications

Bài tập tuần 4 gồm 2 bài, mỗi bài có một server và một client riêng.

| Bài | Server executable | Client executable | Port mặc định |
| --- | --- | --- | --- |
| Bài 1 – String splitter | `bin/server.exe`   | `bin/client.exe`   | `5500` |
| Bài 2 – File transfer  | `bin/server_b2.exe` | `bin/client_b2.exe` | `5600` |

## Cấu trúc thư mục

```
HW4/
├── Makefile           # Makefile dùng cho cả 4 binary
├── .gitignore
├── bin/              # File thực thi sau khi build (.exe trên Windows)
├── build/            # Object + dependency files
├── core/
│   ├── inc/          # Header (.h)
│   └── src/          # Source (.c)
├── logs/             # Log hoạt động: logs/hw4_20235876.log
├── server_storage/   # Thư mục server Bài 2 lưu file upload
├── docs/             # Đề bài PDF
└── Minh Chứng/       # Ảnh chụp khi chạy thực nghiệm
```

## Build & Clean

```bash
mingw32-make            # Build tất cả 4 binary
mingw32-make clean      # Xoá file build + binary
```

Chạy từ thư mục gốc `HW4` để các đường dẫn tương đối (`logs/`,
`server_storage/`) trỏ đúng vị trí.

## Bài 1 – String splitter

Giao thức (framing đầy đủ, đáp ứng yêu cầu "truyền theo dòng byte
trên TCP" của đề):

1. Client gửi 4 bytes length (network byte order) + payload là chuỗi
   người dùng nhập.
2. Server phản hồi:
   - 1 byte status: `0` = OK, `1` = Error.
   - Nếu OK: 4 bytes length của chuỗi letters, chuỗi letters, 4 bytes
     length của chuỗi digits, chuỗi digits.
3. Client lặp cho tới khi người dùng nhập xâu rỗng.

Ví dụ:

```
INPUT   OUTPUT (letters / digits)
1a2b3cd -> "abcd" / "123"
123    -> ""     / "123"
abcd   -> "abcd" / ""
Ab15CD$ -> Error: string contains an invalid character
```

Server / Client phải chạy ở cùng port. Mỗi request được log vào
`logs/hw4_20235876.log` với thẻ `B1`.

## Bài 2 – File transfer

Giao thức:

1. Client mở file, gửi header `ft_header_t` (status, name_len, size)
   rồi tới `name_len` bytes tên file, rồi `size` bytes payload.
2. Server kiểm tra:
   - File đã tồn tại → trả về `FT_STATUS_EXISTS` ("Error: File is
     existent on server").
   - File > 100 MB → trả về `FT_STATUS_TOO_BIG`.
   - Lỗi đọc/ghi giữa chừng → trả về `FT_STATUS_INTERRUPTED`.
   - Thành công → trả về `FT_STATUS_OK`.
3. Server lưu mọi file upload vào thư mục `server_storage/`.
4. Client lặp cho tới khi đường dẫn rỗng.

Ví dụ:

```
INPUT                  OUTPUT
D:/test/testfile.jpg   Successful transfering
D:/test/testfile.rar   Error: File not found
D:/test/testfile.jpg   Error: File is existent on server
D:/test/testfile.avi   Error: File tranfering is interupted
```

Mỗi request được log với thẻ `B2` vào cùng file log trên.

## Tác giả

Hoàng Kim Vĩnh – 20235876