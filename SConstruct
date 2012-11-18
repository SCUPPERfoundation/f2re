
# ibgames fed2 SCons file

# VariantDir('./obj','./src')

env = Environment(CPPPATH='#./include',
						CCFLAGS ='-gstabs+ -Wall -Wcast-qual -Wcast-align -Wwrite-strings -Wpointer-arith -O2 -DDEVELOPMENT',
						LIBS = ['expat','db_cxx','sqlite3'],
						LIBPATH = '/usr/lib')


SConscript(['src/SConstruct'],exports = 'env',variant_dir = './obj', duplicate = 0)
