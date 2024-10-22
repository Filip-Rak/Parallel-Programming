CCOMP = gcc
OPT = -g -O0

SOURCE_1_1_b = pub_sym_1_1_bad.c
SOURCE_1_1 = pub_sym_1_1.c
SOURCE_1_2 = pub_sym_1_2.c
SOURCE_2_1 = pub_sym_2_1.c
SOURCE_2_2 = pub_sym_2_2.c

OUT_1_1b = pub1_1b.out
OUT_1_1 = pub1_1.out
OUT_1_2 = pub1_2.out
OUT_2_1 = pub2_1.out
OUT_2_2 = pub2_2.out

all: $(OUT_1_1) $(OUT_1_2) $(OUT_2_1) $(OUT_2_2) $(OUT_1_1b)

$(OUT_1_1b): $(SOURCE_1_1_b)
	$(CCOMP) $(SOURCE_1_1_b) $(OPT) -o $(OUT_1_1b)

$(OUT_1_1): $(SOURCE_1_1)
	$(CCOMP) $(SOURCE_1_1) $(OPT) -o $(OUT_1_1)

$(OUT_1_2): $(SOURCE_1_2)
	$(CCOMP) $(SOURCE_1_2) $(OPT) -o $(OUT_1_2)

$(OUT_2_1): $(SOURCE_2_1)
	$(CCOMP) $(SOURCE_2_1) $(OPT) -o $(OUT_2_1)

$(OUT_2_2): $(SOURCE_2_2)
	$(CCOMP) $(SOURCE_2_2) $(OPT) -o $(OUT_2_2)


clean:
	rm $(OUT_1_1) $(OUT_1_2) $(OUT_2_1) $(OUT_2_2) $(OUT_1_1b)