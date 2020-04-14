set title 'ROC Eigenfaces_HL' font ',20'
set xlabel 'False Positives'
set ylabel 'True Positives'
set grid xtics lt 0 lw 1 lc rgb '#bbbbbb'
set grid ytics lt 0 lw 1 lc rgb '#bbbbbb'
plot 'plots/ROC Eigenfaces_H_MahalanobisDistance.plot' with lines lw 3 lc rgb 'red', 'plots/ROC Eigenfaces_L_MahalanobisDistance.plot' with lines lw 3 lc rgb 'blue',