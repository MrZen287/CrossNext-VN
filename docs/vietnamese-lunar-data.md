# Dữ liệu lịch âm Việt Nam

CrossNext-VN tính lịch theo UTC+07:00 từ thời điểm sóc và các trung khí.
Tháng chứa ngày đông chí là tháng 11. Trong chu kỳ có 13 tháng giữa hai
tháng 11, tháng đầu tiên sau tháng 11 thiếu trung khí là tháng nhuận.
Các sự kiện được xếp vào tháng theo ngày địa phương.

Bảng lưu sẵn thời điểm Tết, độ dài tháng và tháng nhuận cho các năm âm
lịch 1969–2199. Năm 1969 hỗ trợ việc tra ngày đầu năm dương lịch 1970.
Phạm vi tra ngày dương lịch là 1970–2199. Xa thời điểm hiện tại, các dự
báo thiên văn sát nửa đêm có thể thay đổi theo mô hình thời gian.

Bảng được sinh độc lập bằng [Astronomy Engine 2.1.19](https://github.com/cosinekitty/astronomy)
với [giấy phép MIT](../licenses/AstronomyEngine-MIT.txt). Mã sinh bảng
không cần chạy trên máy đọc sách.

```sh
python -m pip install astronomy-engine==2.1.19
python tools_local/lunar/generate_vietnamese_lunar.py
```

Script kiểm tra ngày Tết và tháng nhuận đã biết, số tháng trong từng năm,
độ dài từng tháng và tính liên tục tại tất cả các ranh giới năm trong bảng.

