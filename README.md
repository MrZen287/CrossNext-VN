> **1.1.1.4:** Sửa lịch khóa lệch/phần viền đáy, khung sách đỉnh3D–đáy phẳng và nuôi cặp Mèo–Mọt. Hai bé dùng chung tiến trình chăm sóc/chơi. **English:** Fixed centered calendar lock/footer border, fixed book shell and illustrated Cat & Worm pair with shared care/play progress. [Ghi chú / Release notes](docs/crosshihi/release-1.1.1.4.md).

# CrossZen 1.1.1.4

Firmware dành riêng cho **Xteink X4 Pro (ESP32-S3)**, phát triển từ CrossNext-VN 2.0.4-vn.1.

**Bản thử công khai:** mã cần vượt qua build X4 Pro và kiểm thử máy tính trước khi phát hành. Chưa có kết quả đo pin, cảm ứng hay ngủ/thức trên thiết bị thật; không coi hình demo là ảnh chụp thiết bị.

## Giao diện

![Ý tưởng giao diện CrossZen](docs/crosshihi/crosszen-demo.png)

*Ảnh trên là demo thiết kế, không phải ảnh chạy firmware. Danh sách APP trong ảnh là minh họa. Mã thực tế giữ danh mục app/game của CrossNext-VN; menu Mọt có thêm Truyền tệp. Linh vật trong firmware dùng bộ hình Mọt/Mèo riêng, chuyển thành mask đen trắng nhỏ để không chạy hoạt ảnh liên tục.*

- Logo khởi động Mọt + Mèo, tên **CrossZen**, phiên bản **1.1.1.4**.
- Home cố định: sách đang đọc lớn, châm ngôn một dòng ở giữa, kệ sách vuốt ở dưới cùng.
- Chạm bìa hoặc thông tin sách chính để đọc tiếp. Chạm sách trên kệ để mở; vuốt để duyệt tiếp thư viện đã lập chỉ mục.
- Mèo lớn ôm bìa có năm tư thế/biểu cảm, đổi khi vào Home; chạm mèo để đổi tư thế. Không chạy hoạt ảnh liên tục.
- Mọt ngồi cố định ở đầu kệ. Popup là lớp phủ, không đẩy châm ngôn hay linh vật.
- Menu Mọt: **Thư viện · Thống kê đọc · Lịch · APP · Truyền tệp · Cài đặt**. Nút Confirm trên Home cũng mở menu khi chưa chọn mục; Back đóng popup.
- APP gộp toàn bộ app và game đang có thành lưới **4 cột × 5 hàng**, vuốt sang trang. Dữ liệu lưu và định danh app/game cũ được giữ. Không có thao tác nhấn giữ để ghim icon.
- Tự ngủ khi đang đọc: giữ trang sách. Bấm nguồn chủ động: hiện lịch hoặc ảnh khóa đã chọn.
- Home cố định; khóa chọn Lịch hoặc một ảnh PNG/BMP từ thư mục; cài đặt font/cỡ chữ/lề đọc sách vẫn còn.

## Linh vật từ mã thực tế

![Bộ hình Mọt và Mèo thực tế](docs/crosshihi/crosszen-mascots.png)

Ảnh này xuất từ đúng các mask hình được nhúng trong firmware; không phải ảnh chụp màn hình máy đọc.

## Tiếng Việt và English

Chọn trong **Cài đặt → Hệ thống → Ngôn ngữ**; xác nhận để khởi động lại.

- **Tiếng Việt:** châm ngôn tiếng Việt, ngày âm ở Home và lịch khóa có ngày âm dưới ngày dương; có năm Can Chi.
- **English:** châm ngôn tiếng Anh, ẩn toàn bộ lịch âm ở cả Home và khóa. Lịch dương và điều hướng dùng tiếng Anh.
- Khi chưa đặt được ngày giờ, màn hình khóa yêu cầu đặt giờ, không tạo ngày âm giả.

## Truyền tệp bằng QR

Mở **Mọt → Truyền tệp**. Chọn kết nối Wi-Fi hoặc phát hotspot, chờ máy chủ sẵn sàng rồi quét mã QR. Điện thoại phải ở đúng mạng. Đây là chức năng truyền tệp web kế thừa, không phải dịch vụ đám mây mới.

## Cài đặt và cập nhật

1. Sao lưu toàn bộ microSD, đặc biệt thư mục ẩn `/.crosspoint/` và dữ liệu app.
2. Tải gói **CrossZen-X4Pro-v1.1.1.4.zip** trong [Releases](https://github.com/MrZen287/CrossNext-VN/releases).
3. Chép firmware `.bin` và tài nguyên vào thẻ, giữ nguyên các giấy phép. Không xóa `/.crosspoint/`.
4. Trên X4 Pro có trình cập nhật qua microSD tương thích, vào **Cài đặt → Hệ thống → Cập nhật firmware** và chọn tệp `.bin`.

Tệp **CrossZen-X4PRO-OTA.bin** là firmware ứng dụng cho X4 Pro, không phải ảnh factory hay bootloader. Không dùng cho X3/X4 thường hoặc thay bảng phân vùng. Gói này không thay đổi phân vùng; máy phải có slot ứng dụng đủ lớn.

**OTA:** vào **Cài đặt → Hệ thống → Kiểm tra cập nhật**. Nguồn cập nhật trỏ về repo này, nhận đúng asset CrossZen và so sánh đủ bốn phần phiên bản. Tài nguyên SD cần cập nhật riêng nếu ghi chú bản phát hành yêu cầu.

## Dung lượng bản build

Build X4 Pro sau tối ưu dùng khoảng **92%** ngân sách ứng dụng; giảm khoảng **247 KiB** so với bản đầu. Hai kiểu chữ hiển thị rất lớn trong app dùng chung cỡ 28, không cắt bộ ký tự hay đổi font đọc sách. Đây là dung lượng flash, không phải mức RAM khi chạy.

## Phát triển và kiểm chứng

```sh
pip install git+https://github.com/pioarduino/platformio-core.git
pip install -r requirements.txt
sh host-tests/crosshihi/run.sh
pio run -e gh_release_x4pro
```

Xem [ghi chú bản 1.1.1.4](docs/crosshihi/release-1.1.1.4.md) và [các app được giữ](docs/crosshihi/apps.md).

## Nguồn và giấy phép

CrossZen kế thừa [CrossNext-VN](https://github.com/MrZen287/CrossNext-VN), [CrossNext](https://github.com/manuelx900/CrossNext), [CrossPlay](https://github.com/ma-r-s/crossplay), [CrossPoint](https://github.com/crosspoint-reader/crosspoint-reader) và FreeInk SDK. Giữ [LICENSE](LICENSE), [THIRD-PARTY-NOTICES.md](THIRD-PARTY-NOTICES.md), giấy phép SDK, font và tài nguyên. Đổi thương hiệu trong giao diện không loại bỏ ghi nhận nguồn và giấy phép kế thừa.

Chi tiết bản 1.1.1.4 và đường cập nhật từ CrossHihi xem [ghi chú phát hành](docs/crosshihi/release-1.1.1.4.md).

## Thú cưng CrossZen

Mục Thú cưng thay chế độ cũ bằng lõi [Book Pet](https://github.com/unipheas/book-pet-x3) chuyển thể cho X4 Pro. Mèo và Mọt lớn lên nhờ chăm sóc/chơi; không gắn với thống kê đọc. Có chơi nhớ vị trí, thức ăn, đồ chơi, mở loài và lưu tiến trình NVS riêng. Giữ giấy phép [MIT gốc](licenses/BookPet-MIT.txt). Không có nút quay lại chế độ thú cũ; dữ liệu cũ trên SD được giữ nhưng không nhập vào thú mới.

---

# English — CrossZen

CrossZen1.1.1.4 is a public preview firmware for **Xteink X4 Pro**, evolved from CrossHihi/CrossNext-VN. Vietnamese and English are built in. The Home screen has a larger cover and cat, a smaller cat/worm pair hugging Continue, one-line quotes, a fixed worm popup, and a single bottom date/time/battery row. English hides lunar dates. The lock screen can show the fixed calendar or a PNG/BMP selected from an SD folder. Automatic reader sleep preserves the page.

APP retains24 entries in a4×5 grid over2 pages. The virtual pet uses an adapted [Book Pet](https://github.com/unipheas/book-pet-x3) care engine with illustrated Cat & Worm sharing care/play progress and no reading rewards. Its progress is stored separately.

**Upgrade:** on CrossHihi1.1.1.1/1.1.1.2, open Worm → Settings → System → Check for updates and confirm1.1.1.4. Existing SD resources do not need to be recopied. Selected lock images must be on your card. For a manual application update, use `CrossZen-X4Pro-v1.1.1.4.bin`; do not treat it as a factory image. The legacy CrossHihi OTA asset name remains for compatibility. Back up your SD card and preserve `/.crosspoint/`.

The images on this page are design concepts, not actual device screenshots. See the [bilingual release notes](docs/crosshihi/release-1.1.1.4.md) for implementation details, known limits and image-picker differences. Builds and host tests are required before release; physical touch, ghosting, battery, sleep/wake and end-to-end OTA have not yet been validated.

Source, SDK, fonts, resources and adapted artwork retain their licenses. [Book Pet MIT license](licenses/BookPet-MIT.txt) and [third-party notices](THIRD-PARTY-NOTICES.md).
