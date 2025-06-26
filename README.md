# 🔥 Su-Genk --  Autonomous FireFighter Wall Following Robot

Su-Genk adalah robot otomatis yang dapat berjalan lurus mengikuti jalur di antara dua dinding dan melakukan turn 90° secara cerdas menggunakan sensor **IMU MPU6050** dan **3 sensor ultrasonik HC-SR04**. Robot ini dirancang untuk tetap berada di tengah jalur dan mengambil keputusan arah secara mandiri.

---

## 🚀 Fitur Utama

- **Kalibrasi IMU otomatis** saat startup, menjadikan arah awal sebagai sudut 0° (referensi lurus).
- **Gerak lurus dengan centering**, menjaga robot tetap di tengah antara dua dinding menggunakan kombinasi sensor IMU dan ultrasonik.
- **Belok otomatis 90°** berdasarkan deteksi dinding depan dan pemilihan arah jalur terbuka.
- **Reset sudut IMU setelah belok**, agar orientasi baru dianggap sebagai arah lurus.
- **Kalibrasi mudah** dengan parameter konstanta dan offset yang dapat disesuaikan.

---

## ⚙️ Cara Kerja Logika Program

1. Robot menggunakan dua motor DC dan dikendalikan oleh Arduino.
2. Saat dinyalakan, **IMU dikalibrasi** untuk menetapkan arah 0° sebagai acuan lurus.
3. Robot berjalan lurus dengan tetap menjaga posisi **di tengah antara dua dinding** menggunakan:
   - Sudut dari IMU (untuk arah)
   - Jarak dari sensor ultrasonik kiri dan kanan (untuk posisi)
4. **Centering** dilakukan dengan menyesuaikan PWM motor kiri dan kanan berdasarkan:
   - Selisih sudut dari arah lurus (`changePWM_sudut`)
   - Selisih jarak dari dinding (`changePWM_jarak`)

5. Menggunakan **complementary filter**, PWM motor akan menjadi total dari **PWM_dasar, changePWM_sudut, dan changePWM_jarak.** :
<pre>
**Rumus PWM akhir**
PWM_kiri = PWM_dasar_kiri + changePWM_sudut_kiri + changePWM_jarak_kiri
PWM_kanan = PWM_dasar_kanan + changePWM_sudut_kanan + changePWM_jarak_kanan
</pre>
6. Jika sudut berada di **domain kiri (1–179°)** → PWM motor kiri ditambah (belok kanan).
7. Jika sudut berada di **domain kanan (181–359°)** → PWM motor kanan ditambah (belok kiri).
8. Jika **jarak sensor < offset (mis. 15 cm)** → PWM motor sisi itu ditambah agar menjauh dari dinding.
9. Jika **sensor depan mendeteksi dinding (≤ 15 cm)**:
- Robot berhenti 1 detik
- Membaca jarak kiri dan kanan
- Jika kanan lebih luas → belok kanan ke sudut **270°**
- Jika kiri lebih luas → tetap belok kanan ke sudut **90°** (karena keterbatasan mekanik)
10. Selama proses belok:
 - **Hanya IMU yang aktif**, sensor ultrasonik dinonaktifkan
 - Setelah sudut target tercapai → robot berhenti 1 detik
 - Sudut IMU di-reset ke 0° untuk orientasi baru
11. Robot kembali berjalan lurus dan mengulang proses saat menemui dinding di depan.

---

## 🛠️ Parameter yang Dapat Dikustomisasi

- `konstantaPWM_sudut_kiri` — sensitivitas koreksi sudut kiri
- `konstantaPWM_sudut_kanan` — sensitivitas koreksi sudut kanan
- `konstantaPWM_jarak_kiri` — sensitivitas koreksi jarak ke dinding kiri
- `konstantaPWM_jarak_kanan` — sensitivitas koreksi jarak ke dinding kanan
- `offset_jarak` — batas minimal jarak agar koreksi posisi diaktifkan (misalnya 15 cm)
- `PWM_dasar_kiri` dan `PWM_dasar_kanan` — kecepatan dasar robot saat lurus
