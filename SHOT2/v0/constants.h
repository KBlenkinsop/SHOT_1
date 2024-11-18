#pragma once


////////////////////////////////////////////////
//// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
////////////////////////////////////////////////


// The maximum number of particles the system can store.
static unsigned const PARTICLE_MAX = 1u << 21;

// set how many particles can be spawned in a SINGLE FRAME.
// Therefore, the more frames you have a second, the more particles will be spawnd!
// With the PARTICLE_SPAWN_RATE set to PARTICLE_MAX, PARTICLE_MAX particles will be always be active every frame.
// With a lower spawn rate the number of active particles can vary greatly between frames.
// We want a consistent number of particles to help give us the best (most reliable) metrics.
// This is not always possible however.
// Therefore, a good metric to consider is how much time it takes to update and render a single particle.
// This will therefore take eliminate the number of particles that can spawn as a factor.
//
// FYI, a much lower spawn rate is preferable in 'real' games to help limit the cost of particle creation/update in any given frame.
static unsigned const PARTICLE_SPAWN_RATE = 1u << 14;

static unsigned const NUM_PARTICLE_TYPES = 3u;


////////////////////////////////////////////////
//// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
////////////////////////////////////////////////
