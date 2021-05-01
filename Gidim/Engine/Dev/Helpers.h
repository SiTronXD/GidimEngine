#pragma once

#define S_RELEASE(x) if(x) { x->Release(); x = NULL; }
#define VECTOR_ADDRESS(x) x.empty() ? 0 : &x[0]