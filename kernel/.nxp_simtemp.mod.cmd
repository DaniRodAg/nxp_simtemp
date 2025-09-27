savedcmd_/home/danie/simtemp/kernel/nxp_simtemp.mod := printf '%s\n'   nxp_simtemp.o | awk '!x[$$0]++ { print("/home/danie/simtemp/kernel/"$$0) }' > /home/danie/simtemp/kernel/nxp_simtemp.mod
