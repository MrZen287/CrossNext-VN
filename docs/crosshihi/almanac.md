# Dữ liệu lịch dân gian CrossZen

Phạm vi dữ liệu bổ sung là 01.01.2026–31.12.2028 (1096 ngày). Ngoài khoảng này, bộ đổi âm–dương Việt Nam vẫn hoạt động trong giới hạn gốc 1970–2199, nhưng giao diện không hiện dữ liệu dân gian chưa có.

## Nguồn và quy ước

- Ngày âm–dương: `VietnameseLunar` sẵn có của firmware, không thay bằng ngày âm Trung Quốc.
- Tiết khí, trực và việc nên/kiêng: tạo từ [6tail/lunar-javascript](https://github.com/6tail/lunar-javascript), giấy phép MIT, có Việt hóa tên 114 mục. Xem `licenses/Lunar-Javascript-MIT.txt`.
- Mỗi dòng dữ liệu lấy tại 00:00 giờ Việt Nam, tương ứng 01:00 UTC+8 trong bộ tính; thời điểm tiết khí của nguồn được lùi một giờ. Trực dùng tháng tiết khí chính xác; việc nên/kiêng dùng `getDayYi(2)` / `getDayJi(2)` của nguồn. Đây là quy ước của bộ dữ liệu, không áp dụng mọi trường phái.
- Hoàng/hắc đạo: vòng 12 thần theo số tháng âm Việt và chi ngày; tháng Giêng/7 khởi Thanh Long ở Tý, mỗi tháng tiến hai chi. Sáu thần hoàng đạo: Thanh Long, Minh Đường, Kim Quỹ, Kim Đường, Ngọc Đường, Tư Mệnh. Tháng nhuận dùng cùng số tháng. Giờ Tý ghi 23–01; chi ngày lấy mốc 00:00, không đổi ngày ở 23:00.
- Ngày kỵ chỉ ghi hai nhóm: Tam nương (3, 7, 13, 18, 22, 27 âm) và Nguyệt kỵ (5, 14, 23 âm). Không tự gán nhãn tổng hợp “tốt cho mọi việc”, không tính theo tuổi cá nhân.
- Các ngày lễ trong app là ngày diễn ra/kỷ niệm, không phải lịch nghỉ hay nghỉ bù chính thức.

Có thể gặp ngày hoàng đạo nhưng một số việc vẫn ở cột nên tránh: các mục thuộc những quy tắc khác nhau và được giữ riêng, không chấm điểm cộng gộp. Nội dung là tham khảo lịch dân gian.

## Dữ liệu và tái tạo

`AlmanacDays.inc` chứa 1096 bản ghi tĩnh, mỗi bản ghi 36 byte: hai mặt nạ 128 bit cho việc nên/kiêng, chỉ mục tiết khí và trực. Tên mục dùng chung trong `AlmanacActivities.inc`. Không nạp toàn bộ bảng vào RAM, không cần mạng hoặc Lua/JavaScript trên máy đọc.

`almanac-source.json` ghi SHA-256 của đầu vào. Để tái tạo, tải đúng `lunar.js` tương ứng rồi chạy:

```
node scripts_local/generate_almanac.cjs /absolute/path/lunar.js
```

Bản ghi và tên mục là dữ liệu dẫn xuất từ nguồn MIT; thuật toán và bảng lịch âm Việt hiện có giữ nguyên giấy phép ban đầu. Ảnh nhánh lá/đồng ruộng/mái nhà là tài nguyên gốc tạo cho CrossZen, được đóng gói thành mask 1-bit 9288 byte.
