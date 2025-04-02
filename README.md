# KFS

## Steps

- le bios
    - Le bios / UEFI cherche le bootloader sur un peripherique de stockage
    - il lit le MBR (master boot record) pour trouver le secteur de boot
- Le bootloader
    - Charge Grub (ou autre) en memoir
    - anylse les fichiers de conf (grub.cfg)
- grub
    - verifie si le kernel est compatible avec le standart Multiboot
    - il place le kernel en memeoir et lui passe la main
- kernel commence son exec

## Compilation

[**Avec quoi on compile ?**](#cross-compilateur)
[**Qu'est ce qu'on compile ?**](#elements-a-compiler)

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

- [doc](http://wiki.osdev.org/GCC_Cross-Compiler)

## Notions

### MBR (master boot record)

Excellente question ! Voyons en détail ce qu’est le **MBR (Master Boot Record)** et son rôle dans le processus de démarrage.  

---

## 📌 **Le MBR (Master Boot Record), c’est quoi ?**  

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

### MBR vs GPT : Une alternative moderne
Le MBR est **un ancien standard**, limité à **4 partitions principales et 2 To de stockage maximum**.  
Les disques modernes utilisent **GPT (GUID Partition Table)**, qui est plus flexible.  
💡 **GPT ne contient pas de MBR**, mais un **"Protective MBR"** pour compatibilité avec les anciens systèmes.

## Tuto

[doc tuto](http://wiki.osdev.org/Bare_Bones)

### Prerequis

- The GNU Linker from Binutils to link your object files into the final kernel. `ld` (to link boot + kernel)
- The GNU Assembler from Binutils (or optionally NASM) to assemble instructions into object files containing machine code. (compile asm)
- The GNU Compiler Collection to compile your high level code into assembly. (gcc)
- The GRUB bootloader to bootload your kernel using the Multiboot boot protocol that loads us into 32-bit protected mode with paging disabled.