setwd("E:\\Documents\\programming\\Qt\\EIS\\Test")

Rs = 10
Rct = 100
Q = 1e-5
N = 0.8

T = 10


i = complex (,0,1)
Freq = 10^seq(6,-3, by=-0.1)
Omega = 2*pi*Freq

Z_CPE = Rs + 1/ (1/Rct + Q * (i*Omega)^N)
FLWs = 80 * tanh(sqrt(i*Omega*T))/sqrt(i*Omega*T)
Z_FLWs= Rs/2 + 1 / (1/ (Rct/3 + FLWs) + 1e-5*i*Omega)

plot (Re(Z_CPE), -Im(Z_CPE), bg="red", pch=21, xlim=c(0, 150))
points (Re(Z_FLWs), -Im(Z_FLWs), bg="yellow", pch=21)

write.table (data.frame(Freq, Re(Z_CPE), Im(Z_CPE)),"CPE(n=0.8).txt", row.names=FALSE, col.names=FALSE, sep="\t")
write.table (data.frame(Freq, Re(Z_FLWs), Im(Z_FLWs)),"FLWs(T=10).txt", row.names=FALSE, col.names=FALSE, sep="\t")
