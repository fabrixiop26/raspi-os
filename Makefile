#Especifico el compilador a usar
ARMGNU ?= "C:\Users\Fabricio & Diego\Documents\FabrixioP26\UNINORTE\2021-10\SistemasOperacionales\gcc-arm-10.2-2020.11-mingw-w64-i686-aarch64-none-elf\bin\aarch64-none-elf"
#Parametros del compilador
#-Wall: show all warnings
#-nostdlib: dont use C standard library, ya que esta libreria interactura con #el OS y como estamos haciendo el OS pues XD
#-nostartfiles: Don't use standard startup files, esto se encarga de iniciar el #stack pointer el entry point etc y como lo estamos haciendo nosotros no tiene #sentido
#-ffreestanding: standard library may not exist, le indica al compilador que #funciones que son "estandar" no tienen la definicion que el esperaria ya que #nosotros las crearemos
#-Iinclude: Search for header files in the include folder ( de ahi que lo creemos)
#-mgeneral-regs-only: Use only general-purpose registers, ARM tiene otro tipo #de registers que complican todo como en context switch
COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only
ASMOPS = -Iinclude

#Evitar que estos targets se confundan por archivos
.PHONY: all clean doc

#Nombre de carpetas que contienen los archivos
BUILD_DIR = build
SRC_DIR = src
KERNEL_SRC = src/kernel
DRIVERS_SRC = src/drivers

#En caso de no espeficiar un target por defecto escoge el primero
#por eso se pone esto al principio, el caul como dependencia tiene a el label #kernel8.img
all: kernel8.img

#Limpia los archivos de compilacion
clean :
	rm -rf $(BUILD_DIR) *.img


#Compila los archivos .c en archivos object .o $< hace referencia al filename #de la primera dependencia y $@ hace referencia al filename del current target
#mkdir -p permite crear sub directories cuando no sabes si existe o no
#-MMD crea un archivo de dependencia para cada .o, este archivo define toda las #dependencias para un archivo fuente (headers en general)
#@D refiere a la ultima carpeta del target
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
#extra es necesario añadir todos los archivos fuente para que se tengan en cuenta en los target de OBJ_FILES
C_FILES += $(wildcard $(KERNEL_SRC)/*.c)
C_FILES += $(wildcard $(DRIVERS_SRC)/*.c)
#finextra
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%.o)

#Array de todos los archivos object usando substitution refs
#see https://www.gnu.org/software/make/manual/html_node/Substitution-Refs.html
DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

#Los archivos .elf estan diseñados para ser ejecutados por un OS si queremos #que sea bare metal toca crear un .img (8 stands for ARMv8 como en Android)
kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf  $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img

#Genera documentacion prefijo con -
#en caso de que doxygen no este instalado y en el PATH
#guardo el output del comando en log.txt para evitar saturacion de consola
doc:
	@echo "Generando docs"
	-doxygen Doxyfile > log.txt
#<target>: <dependecy1> <dependecy2>