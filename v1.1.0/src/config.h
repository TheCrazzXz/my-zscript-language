#ifndef __CONFIG_H__
#define __CONFIG_H__

extern int __zs__dbg_level__;

#define DO_DEBUG(minlevel) (__zs__dbg_level__ >= minlevel)

#endif