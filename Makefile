NAME = kfs

BIN = $(NAME).bin
ISO = $(NAME).iso

ISODIR = isodir/boot

CC = i386-elf-gcc

CFLAGS = -fno-builtin \
		 -fno-exceptions \
		 -fno-stack-protector \
		 -nostdlib \
		 -nodefaultlibs \
		 -Wall \
		 -Werror \
		 -Wextra \
		 -std=gnu99 \
		 -ffreestanding \
		 -O2

LINK_FLAG = -ffreestanding \
			-O2 \
			-nostdlib

CFILES = src/kernel.c

OBJ = $(CFILES:.c=.o)

BOOT_OBJ = $(BOOT:.s=.o)

BOOT = src/boot.s

LINKER = src/linker.ld

GRUB_CFG = grub.cfg

GRUB = grub-mkrescue

I386AS = i386-elf-as

RM = rm -rf

# all: boot $(OBJ) link iso
all : $(ISO)


%.o: %.s
	$(I386AS) $< -o $@
	@echo "$(BLUE)Creating object file -> $(MAGENTA)$(notdir $@)... $(RED)[Done]$(DEF_COLOR)"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "$(BLUE)Creating object file -> $(MAGENTA)$(notdir $@)... $(RED)[Done]$(DEF_COLOR)" 


# link :
$(BIN) : $(BOOT_OBJ) $(OBJ) $(LINKER)
	@echo "$(BLUE)Linking boot + kernel...$(DEF_COLOR)"
	$(CC) -T $(LINKER) -o $(BIN) $(LINK_FLAG) $(BOOT_OBJ) $(OBJ) -lgcc
	@echo "$(RED)[Done]$(DEF_COLOR)"

# iso :
$(ISO) : $(BIN) $(GRUB_CFG)
	@echo "$(BLUE)Creation ISO file...$(DEF_COLOR)"
	@mkdir -p $(ISODIR)/grub
	@cp $(BIN) $(ISODIR)
	@cp $(GRUB_CFG) $(ISODIR)/grub
	$(GRUB) -o $(ISO) isodir 
	@echo "$(RED)[Done]$(DEF_COLOR)"

clean : 
	@$(RM) $(BOOT_OBJ)
	@$(RM) $(OBJ)
	@echo "$(BLUE)$(NAME) files cleaned!$(DEF_COLOR)"

fclean : clean
	@$(RM) $(ISO)
	@$(RM) isodir
	@$(RM) $(BIN)
	@echo "$(BLUE)$(NAME) files fcleaned!$(DEF_COLOR)"

re : fclean all

run : all
	qemu-system-i386 -cdrom $(ISO)

.PHONY: all clean fclean re iso link boot run

NOC = \033[0m
BOLD = \033[1m
UNDERLINE = \033[4m
DEF_COLOR = \033[0;39m
GRAY = \033[0;90m
RED = \033[0;91m
GREEN = \033[0;92m
YELLOW = \033[0;93m
BLUE = \033[0;94m
MAGENTA = \033[0;95m
CYAN = \033[0;96m
WHITE = \033[0;97m
FONT_BOLD := $(shell tput bold)
FONT_RED := $(shell tput setaf 1)
FONT_RESET := $(shell tput sgr0)
FONT_CYAN := $(shell tput setaf 6)