#include "cpu.h"
#include "fontset.h"
#include "display.h"

struct CPU initialize() {
  struct CPU cpu;
  cpu.program_counter = 0x200;
  cpu.stack_pointer = 0x0;

  for (uint8 i = 0; i < 80; i++) {
    cpu.RAM[i] = fontSet[i];
  }

  for (uint16 i = 0; i < 2048; i++) {
    cpu.frame_buffer[i] = 0;
  }

  // testing
  // cpu.stack[cpu.stack_pointer] = 0xab;
  cpu.V[0x0] = 0x3e;
  cpu.V[0x1] = 0x1e;
  cpu.RAM[cpu.program_counter] = 0xd0;
  cpu.RAM[cpu.program_counter + 1] = 0x15;
  cpu.I = 0;

  return cpu;
}

uint16 fetchOpcode(struct CPU cpu) {
  return cpu.RAM[cpu.program_counter] << 8 | cpu.RAM[cpu.program_counter + 1];
}

struct CPU readOpcode(uint16 opcode, struct CPU cpu) {
  printf("pc: %x\n", cpu.program_counter);
  printf("Reading opcode: %x\n", opcode);
  if (opcode == 0x00e0) {
    // handle in gfx module
    clearDisplay(cpu.frame_buffer);
    cpu.program_counter++;
  } else if (opcode == 0x00ee) {
    cpu.program_counter = cpu.stack[cpu.stack_pointer];
    cpu.stack_pointer--;
  } else if (0x0000 <= opcode && opcode < 0x1000) {
    printf("Ignored jump to routine at: %X\n", opcode & 0x0fff);
    cpu.program_counter += 2;
  } else if (0x1000 <= opcode && opcode < 0x2000) {
    cpu.program_counter = opcode & 0x0fff;
  } else if (0x2000 <= opcode && opcode < 0x3000) {
    cpu.stack_pointer++;
    cpu.stack[cpu.stack_pointer] = cpu.program_counter;
    cpu.program_counter = opcode & 0x0fff;
  } else if (0x3000 <= opcode && opcode < 0x4000) {
    if ((cpu.V[(opcode & 0x0f00) >> 8]) == (opcode & 0x00ff)) {
      cpu.program_counter += 2;
    }
  } else if (0x4000 <= opcode && opcode < 0x5000) {
    if ((cpu.V[(opcode & 0x0f00) >> 8]) != (opcode & 0x00ff)) {
      cpu.program_counter += 2;
    }
  } else if (0x5000 <= opcode && opcode < 0x6000) {
    if (cpu.V[(opcode & 0x0f00) >> 8] == cpu.V[(opcode & 0x00f0) >> 4]) {
      cpu.program_counter += 2;
    }
  } else if (0x6000 <= opcode && opcode < 0x7000) {
    uint8 x = opcode & 0x0f00 >> 8;
    uint8 kk = opcode & 0x00ff;
    cpu.V[(opcode & 0x0f00) >> 8] = (opcode & 0x00ff);
    cpu.program_counter += 2;
  } else if (0x7000 <= opcode && opcode < 0x8000) {
    cpu.V[(opcode & 0x0f00) >> 8] += (opcode & 0x00ff);
    cpu.program_counter += 2;
  } else if (0x8000 <= opcode && opcode < 0x9000) {
    executeMathInstruction(opcode, cpu);
    cpu.program_counter += 2;
  } else if (0x9000 <= opcode && opcode <= 0xa000) {
    if ((opcode & 0x000f) != 0) { return cpu; }

    if (cpu.V[(opcode & 0x0f00) >> 8] != cpu.V[(opcode & 0x00f0) >> 4]) {
      cpu.program_counter += 2;
    }
  } else if (0xa000 <= opcode && opcode < 0xb000) {
    cpu.I = opcode & 0x0fff;
    cpu.program_counter += 2;
  } else if (0xb000 <= opcode && opcode < 0xc000) {
    cpu.program_counter = cpu.V[0x0] + (opcode & 0x0fff);
    printf("pc: %x\n", cpu.program_counter);
  } else if (0xc000 <= opcode && opcode < 0xd000) {
    uint8 num = generateRandomNumber();
    cpu.V[(opcode & 0x0f00) >> 8] = num & (opcode & 0x00ff);
    cpu.program_counter += 2;
  } else if (0xd000 <= opcode && opcode < 0xe000) {
    uint8 height = opcode & 0x000f;
    uint8 width = 0x8;
    uint16 pixel;
    cpu.V[0xf] = 0;

    for (uint8 row = 0; row < height; row++) {
      pixel = cpu.RAM[cpu.I + row];
      uint8 y = (cpu.V[(opcode & 0x00f0) >> 4] + row) % DISPLAY_HEIGHT;
      for (uint8 col = 0; col < width; col++) {
        uint8 x = (cpu.V[(opcode & 0x0f00) >> 8] + col) % DISPLAY_WIDTH;

        if (pixel & (0x80 >> col)) {
          uint16 frame = x + (y * 64);
          if (cpu.frame_buffer[frame]) { cpu.V[0xf] = 1; }

          cpu.frame_buffer[frame] ^= 1;
        }
      }
    }

    putFrameBuffer(cpu.frame_buffer);
  } else {
    printf("Cannont match opcode: %x\n", opcode);
  }

  return cpu;
}

void executeMathInstruction(uint16 opcode, struct CPU cpu) {
  uint8 x = (opcode & 0x0f00) >> 8;
  uint8 y = (opcode & 0x00f0) >> 4;
  printf("Vx, Vy: %x %x\n", cpu.V[x], cpu.V[y]);
  switch (opcode & 0x000f) {
  case 0x0:
    cpu.V[x] = cpu.V[y];
    break;

  case 0x1:
    cpu.V[x] |= cpu.V[y];
    break;

  case 0x2:
    cpu.V[x] &= cpu.V[y];
    break;

  case 0x3:
    cpu.V[x] ^= cpu.V[y];
    break;

  case 0x4:
    if (cpu.V[x] + cpu.V[y] > 255) {
      cpu.V[0xf] = 1;
    }
    cpu.V[x] += cpu.V[y];
    break;

  case 0x5:
    if (cpu.V[x] > cpu.V[y]) {
      cpu.V[0xf] = 1;
    } else {
      cpu.V[0xf] = 0;
    }
    cpu.V[x] -= cpu.V[y];
    break;

  case 0x6:
    cpu.V[0xf] = cpu.V[x] & 1;
    cpu.V[x] >>= 1;
    break;

  case 0x7:
    if (cpu.V[y] > cpu.V[x]) {
      cpu.V[0xf] = 1;
    } else {
      cpu.V[0xf] = 0;
    }
    cpu.V[x] = cpu.V[y] - cpu.V[x];
    break;

  case 0xe:
    cpu.V[0xf] = cpu.V[x] & 1 << (sizeof(uint8) - 1);
    cpu.V[x] <<= 1;
    break;

  default:
    printf("Could not match opcode %x\n", opcode);
  }
}

uint8 generateRandomNumber() {
  FILE *random_file = fopen("/dev/urandom", "r");
  if (random_file < 0) {
    printf("Cannot open\n");
    return -1;
  } else {
    uint8 random_data[2];
    ssize_t result = fread(random_data, sizeof(uint8), (sizeof(random_data) / sizeof(uint8)), random_file);
    if (result < 0) {
      printf("cannon read\n");
    } else {
      return random_data[0];
    }
  }

  return -1;
}
