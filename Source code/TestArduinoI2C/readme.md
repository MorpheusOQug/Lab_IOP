# Hướng dẫn chạy test với các màn LCD 1602 16x2

### Màn LCD với Arduino kết nối vs bo mạch sạch  
 Trước khi nối màn hình LCD với bo mạch Arduino của bạn, chúng tôi khuyên bạn nên hàn một dải tiêu đề chân vào đầu nối đếm chân 14 (hoặc 16) của màn hình LCD, như bạn có thể thấy trong hình ảnh phía trên.  

 Để nối dây màn hình LCD với bo mạch của bạn, hãy kết nối các chân sau:  

 - Chân LCD RS tới chân kỹ thuật số 12  
 - LCD Kích hoạt chân sang chân kỹ thuật số 11  
 - Chân LCD D4 sang chân kỹ thuật số 5  
 - Chân LCD D5 sang chân kỹ thuật số 4  
 - Chân LCD D6 sang chân kỹ thuật số 3  
 - Chân LCD D7 sang chân kỹ thuật số 2  
 - LCD R/W pin để GND  
 - Chân LCD VSS tới GND  
 - Chân LCD VCC đến 5V  
 - LCD LED + đến 5V thông qua điện trở 220 ohm  
 - LCD LED- đến GND  
 Ngoài ra, nối một chiết áp 10k đến +5V và GND, với đầu ra (đầu ra) của nó tới chân VO của màn hình LCD (chân 3).  
 - Mạch:  
 C1:  
![!\[Alt text\](image.png)](../../Image/LCD_1.png)  
 C2:  
![ !\[Alt text\](image.png)](../../Image/LCD_2.png)  
 C3:
![ !\[Alt text\](image.png)](../../Image/LCD_3.png)  

Đường link github chạy test đơn giản với Arduino: [Title](ICDwithArduino.ino)  
Đường link github code: [Title](ICDwithArduino.ino)  
 - Mạch với chip I2C:  
![!\[Alt text\](image.png)](../../Image/LCDvsI2C_1.png)  
  
Đường link gihub chạy với chip I2C test đơn giản: [Title](LCD_I2C.ino)  
  
 - Sơ đồ:
![ !\[Alt text\](image.png)](../../Image/LCD_M.png)  


