
# ibgames fed2 SCons file

# VariantDir('./obj','./src')

env = Environment(CPPPATH = ['#./include','/usr/include/libdb4'],
						CCFLAGS = ' -Wall -Wcast-qual -Wcast-align -Wwrite-strings -Wpointer-arith -std=c++98 -O2 -DDEVELOPMENT',
						LIBS = ['expat','db_cxx','sqlite3'],
						LIBPATH = '/usr/lib64/libdb4')


SConscript(['src/SConstruct'],exports = 'env',variant_dir = './obj', duplicate = 0)
