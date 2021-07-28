# Sistema Operativo para Raspberry PI 3

Sistema operativo desarrollado para Raspberry Pi 3 basado en Linux para la clase de Sistemas Operativos de la Universidad del Norte. El proyecto aquí presentado esta fuertemente basado en [Learning operating system development using Linux kernel and Raspberry Pi](https://github.com/s-matyukevich/raspberry-pi-os)

## Notas

El código aquí presente esta diseñado para ser ejecutado en el emulador de [**Qemu**](https://www.qemu.org/).  Por lo que se requiere una modificación mínima si se quiere que funcione en hardware real. Ejemplo de una de los cambios a realizar seria en la secuencia de booteo y la forma de creación de las tablas para la **MMU** en el que usa [Identity Paging ](https://wiki.osdev.org/Identity_Paging) por mencionar algunos.

## Características

- Sistema de paginación de memoria básico.

- Comunicación entre CPU y GPU

- Gráficos [uGUI](https://github.com/achimdoebler/UGUI)

- Procesos

- Creación de hilos de usuario y de kernel
	
- Llamadas al sistema

- Simulacion de programa de usuario

- Memoria virtual

- Interactividad simple a través de una línea de comandos

## Requisitos para compilar y ejecutar
