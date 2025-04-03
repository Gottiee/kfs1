# KFS

## Steps

- le bios
    - Le bios / UEFI cherche le bootloader sur un peripherique de stockage
    - il lit le [MBR (master boot record)](#mbr-master-boot-record) pour trouver le secteur de boot
- Le bootloader
    - Charge Grub (ou autre) en memoir
    - anylse les fichiers de conf (grub.cfg)
- grub
    - verifie si le kernel est compatible avec le standart Multiboot
    - il place le kernel en memeoir et lui passe la main
    - grub saute a l'address _start definie dans [boot.s](./src/boot.s)
- kernel commence son exec

## Compilation

- [doc](http://wiki.osdev.org/GCC_Cross-Compiler)
- [**Avec quoi on compile ?**](#cross-compilateur)
- [**Qu'est ce qu'on compile ?**](#elements-a-compiler)
- [**Creer son propre cross compiler**](#create-your-own-cross-compiler)

### Cross compilateur

- On peut utiliser gcc pour faire un cross compilateur

**A quoi ca sert ? et pk on utilise pas gcc simplement pour compiler notre kernel ?**

#### Pas de bibliothque standart
GCC classique est configuré pour compiler des applications sous Linux, avec glibc (GNU C Library).
👉 Mais ton kernel ne tourne pas sous Linux ! Donc pas de glibc, pas de printf, pas de malloc, etc.

Or, par défaut, gcc insère automatiquement des références à la glibc. Donc si tu compiles normalement, tu obtiendras une erreur à l’exécution, car ces bibliothèques n’existent pas sur ton OS minimaliste.

On utilise un gcc spécial (i386-elf-gcc) qui n’ajoute pas ces dépendances, pour compiler un programme totalement autonome (-ffreestanding).

#### Gestion des multiples architectures et format de binaire

Un gcc normal produirait un exécutable 64 bits Linux ELF, alors que toi, tu veux un binaire 32 bits bootable. Avec un cross-compilateur, on force la bonne architecture (i386-elf-gcc pour un kernel 32 bits).

#### Le kernel c'est pas une program 

Un programme Linux standard (ex. ./a.out) dépend du système d’exploitation. Il utilise :

- syscalls (open, read, write, etc.) → mais ton kernel n’a pas encore de gestion des fichiers !
- Un chargeur dynamique (ld.so) → mais ton kernel doit être autonome !

💡 Solution avec un cross-compilateur :
On désactive toutes ces options (-ffreestanding, -nostdlib, -mno-red-zone) pour que le binaire ne dépende de rien et soit directement exécutable par GRUB.

### Elements a compiler

| Étape | Code source | Résultat compilé |
|-------|------------|------------------|
| 1️⃣ Bootloader | `boot.asm` (ASM) | `boot.o` (objet) |
| 2️⃣ Kernel minimal | `kernel.c` (C) | `kernel.o` (objet) |
| 3️⃣ Link final | `boot.o + kernel.o` | `kernel.bin` (exécutable) |
| 4️⃣ Génération ISO | `kernel.bin` + GRUB | `os.iso` |

### Create your own cross compiler

2 choix:

- telecharger le cross compiler : [prebuilt here](http://wiki.osdev.org/GCC_Cross-Compiler#Prebuilt_Toolchains)
    - choose [i386-elf 7.5.0 target](https://newos.org/toolchains/i386-elf-7.5.0-Linux-x86_64.tar.xz)
- compiler nous meme le cross compiler en suivant les etapes suivantes:
    - [tuto cross compiler](http://wiki.osdev.org/GCC_Cross-Compiler)

#### Dependances:

```sh
sudo apt install build-essential bison flex libgmp -dev libmpc-dev libmpfr-dev texinfo libisl-dev
```

On pourrait avoir besoin de retelecharger GCC et Binutils pour compiler le cross compilateur, si ceux sur linux ne font pas le taf:

- [GCC downaload](https://www.gnu.org/software/gcc/)
- [Binutils download](https://www.gnu.org/software/binutils/)

#### Build

 ```sh
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
 ```

```sh
# j'ai pas tout mis, parce que si ca fonctionne, c'est bcp plus simple de faire de telecharger le cross compiler
```

## Notions

### MBR (master boot record)

Le **MBR** est un tout petit morceau de code stocké **au tout début d’un disque dur ou d’un autre périphérique de stockage (clé USB, SSD, etc.)**. Il **contient les informations nécessaires au boot du système**.  

📍 **Localisation** :  
- Il est **situé dans le tout premier secteur du disque**.  
- Sa taille est exactement **512 octets** (soit un demi-ko).  

📍 **Contenu du MBR** :  
| Partie | Taille | Description |
|--------|--------|------------|
| **Bootloader primaire** | 446 octets | Code d’amorçage qui va charger un bootloader plus complet comme GRUB. |
| **Table des partitions** | 64 octets | Informations sur les partitions du disque (MBR supporte 4 partitions principales). |
| **Signature magique** | 2 octets | Valeur `0x55AA` indiquant que c'est un MBR valide. |

---

**Pourquoi le MBR est important dans le démarrage ?**  

Quand un PC démarre, **le BIOS cherche un bootloader** sur les périphériques de stockage. Voici ce qu’il fait :  

1️⃣ **Le BIOS charge le MBR en mémoire** (les 512 premiers octets du disque).  
2️⃣ **Il exécute le bootloader contenu dans le MBR**.  
3️⃣ **Le bootloader MBR charge un autre bootloader plus avancé** (comme GRUB).  
4️⃣ **GRUB charge ensuite le kernel de ton OS**.  

💡 **Si le MBR n’est pas valide ou absent, le BIOS affichera une erreur du type :**  
```
No bootable device -- insert boot disk and press any key
```

#### MBR vs GPT : Une alternative moderne
Le MBR est **un ancien standard**, limité à **4 partitions principales et 2 To de stockage maximum**.  
Les disques modernes utilisent **GPT (GUID Partition Table)**, qui est plus flexible.  
💡 **GPT ne contient pas de MBR**, mais un **"Protective MBR"** pour compatibilité avec les anciens systèmes.

### VGA Text Mode Buffer

- 0xB8000 est une adresse réservée pour l'affichage texte sur x86.
- Écrire en 0xB8000 met directement du texte à l’écran sans driver.
- Chaque caractère prend 2 octets (ASCII + couleur).
- C’est la façon la plus simple d’afficher quelque chose dans un kernel minimaliste.

## Tuto

[doc tuto](http://wiki.osdev.org/Bare_Bones)

### Prerequis

- The GNU Linker from Binutils to link your object files into the final kernel. `ld` (to link boot + kernel)
- The GNU Assembler from Binutils (or optionally NASM) to assemble instructions into object files containing machine code. (compile asm)
- The GNU Compiler Collection to compile your high level code into assembly. (gcc)
- The GRUB bootloader to bootload your kernel using the Multiboot boot protocol that loads us into 32-bit protected mode with paging disabled.

### Structure

```
src
|____ boot.s (kernel entry point that sets up the processor environment)
|____ kernel.c (kernel routines)
|____ linker.ld (linking above files)
```

### assemble boot

```sh
#or i386 ?
i686-elf-as boot.s -o boot.o 
```

### Write kernel

```sh
i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
```

### Linking Kernel and boot 

```sh
i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc
```

### Verification

Run to verify multiboot

```sh
if grub-file --is-x86-multiboot myos.bin; then
  echo multiboot confirmed
else
  echo the file is not multiboot
fi
```

### Booting the kernel

```sh
mkdir -p isodir/boot/grub
cp myos.bin isodir/boot/myos.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o myos.iso isodir
```

### Launching with QEMU

```sh
qemu-system-i386 -cdrom myos.iso
```