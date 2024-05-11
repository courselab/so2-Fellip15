import sys

class InstructionManager:
    mov_op_code_map = {
        'al': bytes([0xb0]),
        'ah': bytes([0xb4])
    }

    @classmethod
    def __get_hexadecimal_value(cls, value) -> bytes:
        value = value.replace(',', '')
        
        if len(value) > 4:
            first_hex = int(value[:4], 16)
            second_hex = int('0x' + value[4:], 16)
            return bytes([second_hex, first_hex])

        hex_value = int(value.replace(',', ''), 16)
        return bytes([hex_value])

    @classmethod
    def __get_char_value(cls, value: str) -> str:
        return value.replace(',', '').replace('\'', '').encode('utf-8')

    @classmethod
    def __get_register_mov_opcode(cls, op: str) -> bytes:
        if op not in cls.mov_op_code_map.keys():
            return None
        return cls.mov_op_code_map[op]
    
    @classmethod
    def jmp_instruction(cls, output_file):
        output_file.write(bytes([0xeb, 0xfd]))
    
    @classmethod
    def hlt_instruction(cls, output_file):
        output_file.write(bytes([0xf4]))

    @classmethod
    def mov_instruction(cls, instructions, index, output_file):
        value = instructions[index + 1].replace('$', '')
        register = instructions[index + 2].replace('%', '')

        if register[0] == '\'':
            value = '\' \','
            register = instructions[index + 3].replace('%', '')

        if value[0] == '0':
            value = cls.__get_hexadecimal_value(value)
        elif value[0] == '\'':
            value = cls.__get_char_value(value)

        output_file.write(cls.__get_register_mov_opcode(register) + value)

    @classmethod
    def int_instruction(cls, instructions, index, output_file):
        output_file.write(bytes([0xcd]) + cls.__get_hexadecimal_value(instructions[index + 1].replace('$', '')))

    @classmethod
    def word_instruction(cls, instructions, index, output_file):
        output_file.write(cls.__get_hexadecimal_value(instructions[index + 1].replace('$', '')))
    
    @classmethod
    def fill_instruction(cls, output_file):
        output_file.write(bytes(461))

class Main:
    
    instruction_manager = InstructionManager()

    @classmethod
    def make_bin_from_assembly(cls, input_file_name: str, output_file_name: str):
        input_file = open(input_file_name, "r")
        output_file = open(output_file_name, "wb")

        line = input_file.readline()
        while line:
            line = line.rstrip()

            operations = line.split()
            size = len(line.split())

            for index in range(size):
                op = operations[index]

                if op == '#':
                    break
                elif op == 'hlt':
                    cls.instruction_manager.hlt_instruction(output_file)
                elif op == 'mov':
                    cls.instruction_manager.mov_instruction(operations, index, output_file)
                elif op == '.word':
                    cls.instruction_manager.word_instruction(operations, index, output_file)
                elif op == 'int':
                    cls.instruction_manager.int_instruction(operations, index, output_file)
                elif op == 'jmp':
                    cls.instruction_manager.jmp_instruction(output_file)
                elif op == '.fill':
                   cls.instruction_manager.fill_instruction(output_file)

            line = input_file.readline()

        input_file.close()

if __name__ == '__main__':
    Main().make_bin_from_assembly(sys.argv[1], sys.argv[2])
