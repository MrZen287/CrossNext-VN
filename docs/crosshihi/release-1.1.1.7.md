# CrossZen 1.1.1.7 — Xteink X4 Pro

## Tiếng Việt

- Sửa lỗi nền đen/chữ đen ở Thư viện → Tất cả. Kiểm tra tái hiện bằng bộ vẽ 1-bit; chưa xác nhận số sách trên thẻ của người dùng.
- Home thứ hai: số ngày lớn, đồng hồ kim dùng viền vuông widget làm khung; trang bên kia có thống kê đọc rộng. Bấm Widget để chọn riêng từng ô: ngày âm–dương, đồng hồ, thống kê, lịch tháng. Chạm widget mở ứng dụng tương ứng.
- Home có mèo: tên sách, tác giả, chương hiện tại/tổng số chương từ mục lục, thanh tiến độ %, chương còn lại, thời gian đọc hết ước tính. Thiếu mục lục hoặc lịch sử thì báo chưa có dữ liệu; ước tính dùng thời gian tích lũy và phần trăm đọc nên có thể lệch khi đọc lại/nhảy chương.
- Menu đọc EPUB → Thêm → Dòng kẻ khi đọc: bật/tắt, chấm mảnh, chấm đậm, gạch ngắn, gạch chấm, nét liền mảnh. Vị trí và khoảng cách theo thông số font/cỡ chữ; không sửa cấu trúc sách.
- Menu đọc EPUB/XTC → Thêm → Vuốt mép: độ sáng / độ ấm. Chọn riêng hai mép, kéo lên tăng/xuống giảm.
- Bảng kéo xuống phân vùng rõ hơn; giao diện hệ thống dùng Roboto. Giữ nút Home một lần trang tiếp, hai lần về Home mèo; trái lùi/phải tiến.

### Font và cập nhật OTA

Bản này chọn **Bokerlam** làm font đọc. Trước khi cập nhật, giữ thư mục **/fonts/Bokerlam/** gồm Bokerlam-Regular.ttf, Bokerlam-Bold.ttf và Bokerlam-Italic.ttf từ gói nội bộ đã cung cấp. Nếu đã chép từ dev.2/dev.3 thì không cần chép lại. Ba font do người dùng cung cấp không kèm trong bản phát hành công khai này. Nếu thiếu font, máy dùng font dự phòng. XTC là ảnh nên không đổi được font trong ảnh.

Cập nhật qua Cài đặt → Kiểm tra cập nhật. Có thể cài file .bin từ thẻ trong Cài đặt → Thiết bị → Cập nhật chương trình cơ sở. Đây là firmware ứng dụng riêng X4 Pro, không phải ảnh factory. Giữ nguyên dữ liệu và bộ tài nguyên trên thẻ.

Giữ toàn bộ app của 1.1.1.6 (đã bỏ Cờ đam), thống kê, lịch/âm lịch và dữ liệu Mèo–Mọt. Ghi chú này bổ sung các thay đổi của 1.1.1.6. Hình giao diện dùng dữ liệu minh họa, không phải ảnh máy thật.

Đã kiểm tra host, build và toàn vẹn ảnh ESP; chưa thử trực tiếp cảm ứng, OTA đầu-cuối, font hay bố cục mới trên thiết bị thật.

## English

X4 Pro only. Fixes black library rows; adds configurable Home widgets, a square analog clock, TOC-based chapter information and estimated remaining reading time. EPUB guides can be disabled or styled as fine dots, bold dots, dashes, dash-dot or a thin solid rule. Reader menus expose brightness/warmth edge gestures. Quick settings are grouped more clearly; system UI uses Roboto.

The Vietnamese-only device UI is retained. Reading selects the user-supplied Bokerlam family from **/fonts/Bokerlam/** on the SD card. Preserve the three TTF files supplied in the earlier private build; they are not redistributed in this public release. OTA updates firmware only; existing SD assets and saves remain. Missing fonts fall back; bitmap XTC text cannot be restyled. Remaining-time estimates depend on accumulated reading history and progress and may be inaccurate after rereading or jumping.

Host/build/ESP image checks are performed; physical-device behavior and end-to-end OTA are not tested. See the 1.1.1.6 notes for the preceding features.
