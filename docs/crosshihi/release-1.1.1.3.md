# CrossZen 1.1.1.3 — bản thử cho Xteink X4 Pro

Đổi tên CrossHihi thành CrossZen; giữ nguyên dữ liệu sách và 24 ứng dụng/game.

![Demo thiết kế đã duyệt](https://raw.githubusercontent.com/MrZen287/CrossNext-VN/crosshihi/docs/crosshihi/crosszen-demo.png)

Ảnh trên là demo thiết kế, không phải ảnh chụp thiết bị. Màn chọn ảnh trong firmware dùng danh sách và trình duyệt thư mục PNG/BMP; chưa có khung xem trước như hình ý tưởng.

## Thay đổi

- Home có bìa sách lớn, mèo lớn ôm bìa; hình ôm góc dùng bộ nét riêng. Cụm Mèo–Mọt nhỏ ôm nút **Đọc tiếp**, bỏ mũi tên.
- Một hàng status dưới: giờ, thứ/ngày dương và ngày âm, % pin. English ẩn ngày âm.
- Châm ngôn chỉ một dòng, lọc bằng chiều rộng font thực tế. Khi mở popup, câu và bố cục nền giữ nguyên.
- Chạm Mọt mở popup sát phía trên, đổi biểu cảm mà không đổi vị trí. Popup được phép che nội dung phía trên.
- APP 4×5, 24 mục ở hai trang, sắp xếp tiện ích/đọc trước rồi game. Không có nhấn giữ để ghim icon.
- Lịch khóa chỉ tên thứ trên cùng, số ngày lớn, dòng âm lịch; bỏ trăng/sao/mèo. Hai cột cuối tuần đậm hơn.
- **Cài đặt → Hiển thị → Màn hình khóa:** Lịch hoặc Ảnh từ thư mục. Chọn một ảnh PNG/BMP từ bất kỳ thư mục trên SD. Ảnh mất/hỏng quay về lịch; không xóa ảnh gốc. Ảnh được chuyển đen trắng khi hiển thị.
- Tự ngủ khi đang đọc vẫn giữ trang; bấm nguồn chủ động dùng lịch hoặc ảnh đã chọn.
- Giữ 24 mục APP; **Thú cưng** được thay bằng chế độ mới chuyển thể Book Pet. Không còn tùy chọn mở chế độ thú cũ. Không xóa thư mục dữ liệu cũ trên SD.
- Thú mới có ba loài Byte/Mote/Pip, cho ăn, vệ sinh, nghỉ/thức, chơi nhớ vị trí, mua thức ăn, mở thú/đồ chơi và nhật ký. Thưởng, kinh nghiệm và mở khóa từ chăm sóc/chơi, **không phụ thuộc đọc sách**; không đọc kho thống kê đọc. Tiến trình lưu riêng trong NVS, không dùng dữ liệu thú cũ.
- Không thêm hẹn giờ/sổ trích dẫn. Đồng bộ statusbar sang toàn bộ Cài đặt/trình đọc để đợt sau.

![Mask linh vật nhúng thực tế](https://raw.githubusercontent.com/MrZen287/CrossNext-VN/crosshihi/docs/crosshihi/crosszen-mascots.png)

## Cập nhật OTA từ CrossHihi

Từ 1.1.1.1/1.1.1.2: **Mọt → Cài đặt → Hệ thống → Kiểm tra cập nhật**, kết nối Wi-Fi rồi xác nhận 1.1.1.3. OTA chỉ cập nhật firmware; không cần chép lại tài nguyên đang có. Asset `CrossHihi-X4PRO-OTA.bin` được giữ làm tên tương thích cho máy cũ, nội dung là CrossZen mới, giống hệt `CrossZen-X4PRO-OTA.bin`.

Ảnh khóa tự chọn phải có sẵn trên thẻ. Chép qua thẻ hoặc **Mọt → Truyền tệp**, sau đó chọn trong cài đặt. Không cần tải lại toàn bộ gói tài nguyên chỉ để đổi ảnh.

Cài qua SD: dùng `CrossZen-X4Pro-v1.1.1.3.bin` với trình cập nhật ứng dụng X4 Pro tương thích. Chỉ dùng cho X4 Pro, không phải ảnh factory. Sao lưu SD trước khi cập nhật, giữ thư mục ẩn `/.crosspoint/`. Gói ZIP kèm tài nguyên kế thừa được cung cấp cho cài mới.

## Kiểm chứng

Build X4 Pro, kiểm thử máy tính và kiểm tra ảnh ESP được chạy trước khi phát hành. Chưa có thiết bị thật để kiểm tra cảm ứng, ghosting, pin, ngủ/thức hoặc OTA xuyên suốt. Đây là bản thử công khai. Xem log build đính kèm để biết dung lượng flash/RAM tĩnh; không suy diễn flash thành mức dùng RAM lúc chạy.

Giữ LICENSE và các ghi nhận nguồn kế thừa. Các sprite CrossZen được tạo riêng, nhúng thành mask ink/silhouette để không xóa hình chữ nhật quanh mèo. Các định danh nội bộ `.crosspoint` và tên branch `crosshihi` giữ nguyên nhằm tương thích dữ liệu/OTA.

## Nguồn chế độ thú mới

Chuyển thể lõi chăm sóc, lưu trạng thái và sprite từ [Book Pet](https://github.com/unipheas/book-pet-x3), MIT, ©2026 Brian Phillips và cộng tác viên. Đã thay lớp hiển thị/điều khiển để chạy trong CrossZen X4 Pro, bỏ engine thưởng đọc sách, đổi mở khóa sang chăm sóc/chơi, dùng vùng lưu `crosszen-pet`. Không dùng bootloader, bảng phân vùng hoặc trình cập nhật X3. Giấy phép đầy đủ: `licenses/BookPet-MIT.txt`. Hình thú trong bản chạy là pixel-art Byte/Mote/Pip từ dự án gốc; hình mèo trong demo trước là ý tưởng thiết kế, chưa phải ảnh giao diện game thực tế.
