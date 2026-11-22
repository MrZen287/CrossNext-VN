# CrossZen 1.1.1.4 — Sửa lịch khóa, khung sách và Mèo–Mọt

Bản thử dành riêng **Xteink X4 Pro**. Cập nhật OTA từ1.1.1.1/1.1.1.2/1.1.1.3; không cần chép lại tài nguyên SD.

![Home demo](https://github.com/MrZen287/CrossNext-VN/releases/download/v1.1.1.4/crosszen-demo.png)

Ảnh demo thiết kế đã duyệt, không phải ảnh chụp thiết bị. / Approved design concept, not a device screenshot.

## Tiếng Việt

- **Sửa lịch khóa trên thiết bị thật:** bỏ lỗi lề48px áp nhầm bên trái; bố cục lấy trực tiếp chiều rộng/cao màn hình, hai lề cân nhau. Footer nằm trong khung, có gạch phân cách và viền đáy bên dưới giờ/%pin, không còn xóa viền khi vẽ.
- **Khung sách Home cố định:** mặt trước không đổi theo tỷ lệ ảnh, bìa được ép kín mặt khung. Có mặt đỉnh tạo chiều sâu và gáy mảnh; cạnh đáy phẳng, không vẽ mặt trang giấy phía dưới. Mèo bám góc khung thay vì bám mép ảnh bìa.
- **Mèo & Mọt:** thay hẳn hình thú pixel Byte/Mote/Pip bằng hai linh vật nét vẽ cùng phong cách Home. Hai bé ở chung nhà, dùng chung tiến trình/chỉ số chăm sóc, có tư thế ngủ, chơi, vui và tương tác vuốt ve. Không còn màn chọn loài pixel. Thức ăn/xu/cấp độ/đồ chơi đang lưu của1.1.1.3 được giữ.
- Vẫn có cho ăn, vệ sinh, nghỉ/thức, chơi nhớ vị trí, mua thức ăn, đồ chơi và nhật ký. Chăm sóc/chơi giúp tăng trưởng; **không dùng thời gian hoặc tiến độ đọc sách** để tăng cấp. Bộ hình nhúng sẵn trong firmware, không cần gói ảnh SD mới.
- Giữ24 mục APP, bản Việt/Anh và quy tắc tự ngủ từ trình đọc giữ trang. English tiếp tục ẩn lịch âm.

### Cập nhật

**Mọt → Cài đặt → Hệ thống → Kiểm tra cập nhật**, kết nối Wi-Fi và xác nhận1.1.1.4. Tên tệp `CrossHihi-X4PRO-OTA.bin` vẫn là alias tương thích cho các bản cũ, nội dung giống hệt tệp CrossZen mới. Ảnh khóa tự chọn vẫn dùng ảnh sẵn trên thẻ. Nếu cài qua SD, dùng `CrossZen-X4Pro-v1.1.1.4.bin` với trình cập nhật ứng dụng X4 Pro tương thích; không phải file factory. Sao lưu thẻ và giữ`/.crosspoint/`.

### Kiểm chứng

Có kiểm thử hình học cho lề trái/phải, vùng nội dung/footer và viền đáy ở nhiều kích thước; kiểm thử care/game/mở khóa/lưu-khôi phục, kiểm tra danh mục24 app và build X4 Pro. Nhật ký build kèm theo ghi dung lượng flash/RAM tĩnh. Chưa có thiết bị kết nối để kiểm tra lại e-ink, cảm ứng, pin và OTA xuyên suốt; cần đối chiếu bản sửa này trên máy thật.

---

## English

CrossZen1.1.1.4 is an **Xteink X4 Pro public preview** fixing the calendar lock, book frame and virtual-pet artwork.

- **Calendar lock fix:** corrects the48px inset accidentally applied to the left edge. Layout now uses explicit screen dimensions and symmetric margins. The footer sits inside the frame with a separator and a visible bottom border; painting it no longer erases the frame.
- **Fixed Home book shell:** every cover fills the same front rectangle. Only the top plane and narrow spine provide depth; the lower edge is flat, with no visible underside/page block. The cat stays anchored to the frame corner regardless of image aspect ratio.
- **Cat & Worm:** replaces the Byte/Mote/Pip pixel artwork and species picker with CrossZen's illustrated companions. They share one care profile, with sleeping, playful, happy and petting reactions. Existing1.1.1.3 food, coins, level and toy progress is preserved.
- Feeding, cleaning, rest/wake, the position-memory game, food purchases, toys and diary remain. Growth is driven by care/play, **not reading time or reading progress**. All artwork is embedded; no SD sprite/resource recopy is required.
- All24 APP entries and Vietnamese/English remain. Automatic reader sleep preserves the page. English hides lunar dates.

### Upgrade

On1.1.1.1/1.1.1.2/1.1.1.3 open Worm → Settings → System → Check for updates, connect to Wi-Fi and confirm1.1.1.4. The legacy`CrossHihi-X4PRO-OTA.bin` alias remains upgrade-compatible and contains the same firmware as the CrossZen asset. No SD resource recopy is needed. Selected lock images must remain on the card. For a manual application update use`CrossZen-X4Pro-v1.1.1.4.bin`; it is not a factory image. Back up the card and preserve`/.crosspoint/`.

### Validation and credits

Geometry tests cover symmetric margins, content/footer separation and preservation of the bottom border at multiple sizes. Care/game/unlock/save-restore,24-entry catalog and X4 Pro build checks run before publication. See the build log for flash/static RAM usage. Touch, e-ink behavior, battery and end-to-end OTA still need physical testing.

The care/state engine remains adapted from Book Pet (https://github.com/unipheas/book-pet-x3), MIT; full license in`licenses/BookPet-MIT.txt`. The new displayed characters use original CrossZen mascot masks. Existing SDK, font, resource and upstream notices remain.
