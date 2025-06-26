Ini adalah logika program robot sebagai gambaran cara kerja kodenya

Robot menggunakan dua motor DC yang dikendalikan oleh Arduino dan sensor IMU MPU6050 untuk navigasi arah.

Sensor IMU dikalibrasi saat awal menyala, sehingga sudut awal dianggap sebagai 0° dan digunakan sebagai referensi utama untuk arah lurus.

Robot bergerak lurus sambil menjaga posisinya tetap di tengah antara dua dinding menggunakan kombinasi sensor IMU dan tiga sensor ultrasonik HC-SR04 (kiri, kanan, dan depan).

Proses centering dilakukan dengan menyesuaikan PWM motor kiri dan kanan berdasarkan data sudut dari IMU dan jarak dari sensor ultrasonik.

Sudut dari IMU digunakan untuk mendeteksi apakah robot melenceng ke kiri atau ke kanan dari arah 0°, lalu menghitung perubahan PWM motor untuk mengoreksi arah.

Jika sudut berada pada domain kiri (1–179°), maka PWM motor kiri ditambah agar robot mengarah kembali ke kanan (lurus).

Jika sudut berada pada domain kanan (181–359°), maka PWM motor kanan ditambah agar robot mengarah kembali ke kiri (lurus).

Perubahan PWM akibat sudut dihitung menggunakan rumus changePWM = |sudut saat ini - titik referensi| × konstanta, dan konstanta ini bisa diatur secara manual.

Sensor ultrasonik digunakan untuk menjaga jarak robot dari dinding kiri dan kanan, khususnya jika terlalu dekat.

Jika jarak dari salah satu sisi kurang dari offset tertentu (misalnya 15 cm), maka PWM motor di sisi tersebut akan ditambah untuk menjauhkan robot dari dinding.

Perubahan PWM akibat jarak juga dihitung menggunakan rumus changePWM = |offset - jarak terbaca| × konstanta, dan memiliki konstanta terpisah dari konstanta sudut.

Total PWM akhir untuk masing-masing motor dihitung dari PWM dasar + perubahan akibat sudut + perubahan akibat jarak.

Jika sensor depan mendeteksi dinding dalam jarak ≤ 15 cm, robot akan berhenti selama 1 detik untuk melakukan analisis belokan.

Saat berhenti, robot membandingkan jarak dari sensor kiri dan kanan untuk menentukan arah jalur terbuka.

Jika jalur lebih terbuka di kanan, robot akan berbelok kanan hingga mencapai sudut 270° berdasarkan IMU.

Jika jalur lebih terbuka di kiri, robot tetap akan berbelok ke kanan hingga mencapai sudut 90° karena keterbatasan mekanik pada arah belok.

Selama proses belok, hanya IMU yang digunakan sebagai acuan, sedangkan sensor jarak tidak aktif untuk centering.

Setelah selesai belok, robot akan berhenti selama 1 detik lalu mereset sudut IMU kembali ke 0° sebagai referensi baru.

Robot kemudian melanjutkan kembali pergerakan lurus dan proses centering, dan siklus akan berulang saat bertemu dinding lagi.

Semua konstanta untuk perubahan PWM berdasarkan sudut dan jarak serta nilai offset dapat dikalibrasi untuk penyesuaian performa robot.
