import random
matrix1_32x32 = [[random.randint(1, 100) for _ in range(256)] for _ in range(256)]

matrix2_32x32 = [[random.randint(1, 100) for _ in range(256)] for _ in range(256)]



# Combine the two matrices into 64 lines of text

text_output_32x32 = "\n".join(

    " ".join(map(str, row)) for matrix in (matrix1_32x32, matrix2_32x32) for row in matrix

)



# Save the output for use

print(text_output_32x32)