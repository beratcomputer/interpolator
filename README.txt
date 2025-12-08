CSV Interpolator Tools
======================

Bu klasörde iki farklı interpolasyon aracı bulunmaktadır:

1. interpolator_standard.exe
   - Kaynak kodu: main_standard.c
   - İşlev: CSV dosyasını okur ve 0.01s aralıklarla lineer interpolasyon yapar.
   - Özellik: Zaman kaydırması YAPMAZ. Veriler olduğu gibi işlenir.
   - Çıktı dosyası: output_interpolated_standard.csv

2. interpolator_timeshift.exe
   - Kaynak kodu: main_timeshift.c
   - İşlev: CSV dosyasını okur ve 0.01s aralıklarla lineer interpolasyon yapar.
   - Özellik: Yaw ekseni HARİÇ diğer tüm eksenleri 200ms geciktirir (Time Shifting).
   - Çıktı dosyası: output_interpolated.csv

Kullanım:
Herhangi bir exe'yi çalıştırın ve girdi CSV dosyasının adını girin (örn: test_keyfi.csv).
Pencere kapanmadan önce sonucu görebilmeniz için işlem sonunda duraklama eklenmiştir.
