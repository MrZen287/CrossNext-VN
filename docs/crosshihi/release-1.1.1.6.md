# CrossZen 1.1.1.6 — Xteink X4 Pro

## Tiếng Việt

Bản thử nghiệm dành riêng cho X4 Pro. Giao diện firmware chỉ còn tiếng Việt; phần ghi chú phát hành vẫn có bản tiếng Anh.

- **Home:** đồng hồ và ngày âm–dương ở góc phải trong khung lịch; phần trăm pin trong bóng suy nghĩ của Mọt; nút Đọc tiếp dạng mây. Tên sách hạ xuống tránh mèo. Vuốt trái ngoài kệ mở Thư viện, vuốt phải mở APP; vuốt trong kệ vẫn chuyển sách.
- **Home thứ hai:** vuốt từ dưới lên tại Home có mèo để mở lưới ứng dụng 4 × 3. Dock cố định gồm Thư viện, Thống kê đọc, Lịch, Cài đặt, không có thanh navigation. Trang đầu có widget ngày dương/âm và thống kê đọc 7 ngày; vuốt trái chuyển trang và thay chúng bằng một widget lịch tháng rộng. Chạm widget mở đúng app Lịch/Thống kê. Vuốt xuống hoặc bấm Home trở lại Home có mèo. Thống kê dùng bộ đệm lịch sử hiện có, chỉ tải khi vào màn hoặc đổi ngày, không quét lại mỗi phút.
- **Phím khi đọc:** Home một lần sang trang, hai lần trong khoảng 350 ms về Home có mèo; nhấn giữ mở thiết lập đọc. Phím trái lùi/phải tiến trang. Áp dụng riêng trang đọc, kể cả máy đã có cấu hình cũ; menu và ứng dụng giữ cách điều khiển riêng.
- **Vuốt mép khi đọc:** kéo dọc mép trái tăng/giảm sáng, mép phải tăng/giảm độ ấm; đổi hoặc tắt trong Cài đặt → Điều khiển. Nhận kéo chậm và hủy khi ra khỏi dải mép; không đồng thời lật trang. Lần nâng cấp đầu bật hai giá trị mặc định mới; lần sau giữ lựa chọn người dùng. Tham khảo [CrossInk EdgeSlide](https://github.com/uxjulia/CrossInk/blob/main/src/util/EdgeSlide.h), triển khai trên bộ nhập của CrossZen.
- **Menu Mọt:** Thống kê đọc, Lịch, Truyền tệp, Cài đặt. Mọt và popup giữ nguyên vị trí, nội dung phía trên có thể bị che.
- **Thư viện:** mở mặc định ở Tất cả sách trong chỉ mục; có tab Gần đây riêng. Quét thư mục sâu tối đa 16 cấp thay cho 5; vẫn có giới hạn 4096 sách và các định dạng được hỗ trợ. Không có thẻ của người dùng để xác nhận số sách thực tế.
- **Điều khiển:** các thanh dùng chung chuyển sang nền trắng, Back/giờ/% pin ở dưới; bỏ icon pin ở thanh trạng thái trình đọc. Bảng kéo xuống có nút tròn, ô Ngày/Đêm và nút gạt bo tròn. Bỏ các lối chọn phong cách Home, giữ lựa chọn font đọc.
- **Khóa:** lịch có khung tờ giấy, lỗ đóng gáy, nét xé giấy và hình thoi; thêm nhánh lá, đồng ruộng và mái nhà quanh số ngày lớn. Không hiện giờ/pin. Vẫn chọn được ảnh PNG/BMP cá nhân.
- **Tự ngủ từ trình đọc:** giữ nguyên bố cục trang, vẽ thêm mèo–mọt đọc sách ở góc dưới. Bấm nguồn chủ động mở màn khóa.
- **Thống kê đọc:** cả bốn tab có biểu đồ hoặc thanh so sánh. Hôm nay dùng sáu khung 4 giờ có sẵn trong lịch sử; Thống kê có tổng theo tháng và nhịp 7 ngày; Lịch có bốn mức độ đọc; Sách có thanh thời gian và vẫn mở được chi tiết. Các số liệu được lấy từ dữ liệu trên máy. Nguồn nhập thiếu dữ liệu chi tiết không được giả thành lịch sử đã biết.
- **Lịch:** chọn nhanh tháng/năm, nút Hôm nay, ngày lễ và tra ngày âm/dương. Chạm ngày mở tờ lịch ngày; vuốt đổi ngày, xem đầy đủ các việc nên làm/kiêng, Can Chi, tiết khí, trực, giờ hoàng đạo và hai nhóm ngày kỵ dân gian.
- **Dữ liệu dân gian:** ngoại tuyến 2026–2028 (1096 ngày), bảng khoảng 39 KB cộng tên mục; ngoài khoảng này vẫn đổi được âm–dương nhưng không bịa dữ liệu tốt/xấu. Nguồn/quy ước: [Almanac](almanac.md). Đây là cách ghi theo lịch dân gian, không phải mức đánh giá đúng cho mọi công việc hoặc từng người.
- **Châm ngôn:** 400 câu tiếng Việt riêng biệt từ 7 từ trở lên; đo theo font để chọn câu đầy đủ vừa một dòng. Không cắt câu, không thêm chữ để đủ độ dài. [Nguồn](quote-sources.json).
- **Ứng dụng:** giữ Mèo–Mọt và tiến trình chăm sóc/chơi hiện tại; bỏ Cờ đam, còn 23 mục app/game. Không tích hợp app 口袋宠物 của firmware gốc.
- **Trình đọc:** thêm nét chấm dưới các dòng EPUB. Bộ đọc hiện không nạp font nhúng qua `@font-face`; bản này dùng hệ font sẵn có/đã cài trên thẻ, không kèm Bookerly. Không thay cấu trúc tệp sách và nhật ký đọc.

### Cài đặt

Máy đang dùng CrossZen/CrossHihi có thể cập nhật qua **Mọt → Cài đặt → Kiểm tra cập nhật** sau khi bản phát hành và `ota.json` đã sẵn sàng. Tên tệp cũ `CrossHihi-X4PRO-OTA.bin` được giữ làm bí danh, cùng nội dung với `CrossZen-X4PRO-OTA.bin`.

Cài từ thẻ: chép `CrossZen-X4Pro-v1.1.1.6.bin` lên microSD rồi chọn **Cài đặt → Thiết bị → Cập nhật chương trình cơ sở**. Máy phải đang có trình cập nhật X4 Pro tương thích; đây là ảnh ứng dụng, không phải factory/bootloader. Từ bản 1.1.1.4 không cần chép lại bộ tài nguyên SD. Giữ nguyên dữ liệu và thư mục ẩn trên thẻ.

### Kiểm tra và giới hạn

Build X4 Pro; kiểm tra phiên bản, bố cục/vùng bấm Home, thanh trạng thái/nút tròn, bốn tab Thống kê, các trang Lịch, phạm vi bảng 1096 ngày và lưu/khôi phục game; kiểm tra định danh board, kích thước slot, checksum và SHA-256 của ảnh ESP32-S3. Ảnh giao diện dựng trên máy tính dùng font/asset trong mã, có dữ liệu minh họa, không phải ảnh chụp thiết bị. Chưa kiểm tra cảm ứng, bóng mờ, pin hoặc cập nhật OTA đầu-cuối trên máy thật.

## English

Experimental **X4 Pro only** firmware. The device UI is Vietnamese only; these release notes remain bilingual.

- Redesigned Home clock/date, worm battery thought bubble and cloud Continue button. Swipe left outside the shelf for Library; right for APP. Shelf swipes still page books. The fixed worm popup has Statistics, Calendar, File transfer and Settings.
- Second Home: swipe up from Cat Home for a 4×3 app grid and a fixed four-icon dock. The first page shows date/lunar and reading widgets; the second replaces both with one full-width monthly calendar. Widgets open Calendar or Reading Statistics. Swipe down or press Home to return. No navigation pill.
- Reader controls: single Home advances, double Home returns to Cat Home, hold opens reading settings; left/back and right/next. Edge drags adjust brightness/warmth with configurable bindings and slow-drag recognition inspired by CrossInk.
- Library opens All indexed books rather than Recent; scan depth increased from 5 to 16. The 4096-record and supported-format limits remain.
- Shared white menu chrome moves Back/time/battery percentage to the bottom. Circular quick settings controls; Home style selection paths removed.
- Paper calendar lock with an original botanical/countryside illustration, no clock/battery. Reader timeout preserves the page and overlays Cat/Worm; deliberate power sleep shows the lock.
- Reading statistics use real stored data: time-of-day bars, monthly totals, seven-day trends, activity heatmap and per-book comparison bars. Existing detail screens and storage are retained.
- Calendar adds month/year picking, holiday browsing and illustrated day sheets with offline almanac details for **2026–2028**. Vietnamese lunar dates are retained; traditional rules and source limitations are documented in [almanac.md](almanac.md).
- 400 distinct Vietnamese sayings/advice lines of at least seven words, with measured single-line fitting. Existing Cat/Worm game retained; Checkers removed; 23 app/game entries remain.
- EPUB dotted guides added. Embedded EPUB font loading and bundled Bookerly are **not** included; installed/built-in reader fonts remain available.

Use the built-in updater once this release and the OTA manifest are published, or install the versioned application `.bin` through the existing X4 Pro SD updater. Existing 1.1.1.4 SD resources do not need to be copied again. Both legacy and CrossZen OTA aliases contain identical bytes. Host/build/image integrity checks are performed; physical-device battery, touch, ghosting and end-to-end OTA are not measured.
