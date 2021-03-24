#pragma once

#define S_RELEASE(x) if(x) { x->Release(); x = NULL; }