#include "systems.h"
#include "cflags.h"
#include "xi.h"

system_t system_init(void f(SYSTEM_ARG_REQUIREMENTS), uint32_t n, ...){
	system_t sys;
	sys.mask = vu64_tInit();
	sys.filter = 0;
	sys.magnet = 0;
	sys.requirement = 0;
	sys.func = f;
	uint32_t i;
	va_list v;
	va_start(v,n);
	for (i = 0;i<n;++i){
		maskAddBit(&sys.mask, va_arg(v, uint32_t));
	}
	system_add_filter(&sys, 1, ENTITY_DEACTIVATED);
	va_end(v);
	return sys;
}

void system_add_requirement(system_t* sys, uint32_t n, ...){
	uint32_t i;
	va_list v;
	va_start(v, n);
	for (i = 0;i<n;++i){
		sys->requirement= bit_on(sys->requirement, va_arg(v, uint32_t));
	}
	va_end(v);

}

void system_remove_requirement(system_t* sys, uint32_t n, ...){
	uint32_t i;
	va_list v;
	va_start(v, n);
	for (i = 0;i<n;++i){
		sys->requirement= bit_off(sys->requirement, va_arg(v, uint32_t));
	}
	va_end(v);
}

void system_add_filter(system_t* sys, uint32_t n, ...){
	uint32_t i;
	va_list v;
	va_start(v, n);
	for (i = 0;i<n;++i){
		sys->filter = bit_on(sys->filter, va_arg(v, uint32_t));
	}
	va_end(v);
}

void system_remove_filter(system_t* sys, uint32_t n, ...){
	uint32_t i;
	va_list v;
	va_start(v, n);
	for (i = 0;i<n;++i){
		sys->filter = bit_off(sys->filter, va_arg(v, uint32_t));
	}
	va_end(v);
}

void system_add_magnet(system_t* sys, uint32_t n, ...){
	uint32_t i;
	va_list v;
	va_start(v, n);
	for (i = 0;i<n;++i){
		sys->magnet = bit_on(sys->magnet, va_arg(v, uint32_t));
	}
	va_end(v);
}

void system_remove_magnet(system_t* sys, uint32_t n, ...){
	uint32_t i;
	va_list v;
	va_start(v, n);
	for (i = 0;i<n;++i){
		sys->magnet = bit_off(sys->magnet, va_arg(v, uint32_t));
	}
	va_end(v);
}

uint8_t system_filter(system_t* sys, uint64_t targetFlag){
	uint64_t t = -1;
	//((((targetFlag & sys->filter) != sys->filter) || (sys->filter==0)) && (((targetFlag & sys->magnet) ==sys->magnet) || (sys->magnet==0)));
	return (
		(targetFlag & sys->magnet) |
		(targetFlag & (~sys->filter)) |
		((~targetFlag) & (~sys->requirement))
	) == t;
}

uint8_t system_layer_check(int32_t layer, entity_data* medium, uint32_t eid){
	return (layer == -1) || (layer == mu32_u32Get(&medium->ent2layer, eid).val);
}

void system_run(system_t* sys, xi_utils* xi, int32_t layer){
	uint32_t archetype;
	for (archetype = 0;archetype<xi->ecs->archetypes.size;++archetype){
		archetype_v2* arch = varch_tRefTrusted(&xi->ecs->archetypes, archetype);
		if (maskCompare(&sys->mask, &arch->mask)){
			system_run_archetype(sys, arch, xi, layer);
		}
	}
}

void system_run_archetype(system_t* sys, archetype_v2* arch, xi_utils* xi, int32_t layer){
	uint32_t entity;
	for (entity = 0;entity<arch->data.size;++entity){
		uint32_t eid = vu32_tGet(&arch->ids, entity);
		if (system_filter(sys, mu32_maskGet(&xi->ecs->masks, eid).val)&&system_layer_check(layer, xi->ecs, eid)){
			sys->func(xi, vu32_tGet(&arch->ids, entity), (vec_t*)mat_tRef(&arch->data, entity), &arch->bit2index);
		}
	}
}

void system_free(system_t* sys){
	vu64_tFree(&sys->mask);
}

