/*
    ChibiOS/GFX - Copyright (C) 2012, 2013
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS/GFX.

    ChibiOS/GFX is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/GFX is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * @file    src/gqueue/gqueue.c
 * @brief   GQUEUE source file.
 */

#include "gfx.h"

#if GFX_USE_GQUEUE

#if GQUEUE_NEED_ASYNC
	void gfxQueueASyncInit(gfxQueueASync *pqueue) {
		pqueue->head = pqueue->tail = 0;
	}
	gfxQueueASyncItem *gfxQueueASyncGet(gfxQueueASync *pqueue) {
		gfxQueueASyncItem	*pi;

		if (!pqueue->head) return 0;
		gfxSystemLock();
		if ((pi = pqueue->head))
			pqueue->head = pi->next;
		pi->next = 0;
		gfxSystemUnlock();
		return pi;
	}
	void gfxQueueASyncPut(gfxQueueASync *pqueue, gfxQueueASyncItem *pitem) {
		pitem->next = 0;

		gfxSystemLock();
		if (!pqueue->head) {
			pqueue->head = pqueue->tail = pitem;
		} else {
			pqueue->tail->next = pitem;
			pqueue->tail = pitem;
		}
		gfxSystemUnlock();
	}
	void gfxQueueASyncPush(gfxQueueASync *pqueue, gfxQueueASyncItem *pitem) {
		gfxSystemLock();
		pitem->next = pqueue->head;
		pqueue->head = pitem;
		if (!pitem->next)
			pqueue->tail = pitem;
		gfxSystemUnlock();
	}
	void gfxQueueASyncRemove(gfxQueueASync *pqueue, gfxQueueASyncItem *pitem) {
		gfxQueueASyncItem *pi;

		if (!pitem) return;
		gfxSystemLock();
		if (pqueue->head) {
			if (pqueue->head == pitem) {
				pqueue->head = pitem->next;
				pitem->next = 0;
			} else {
				for(pi = pqueue->head; pi->next; pi = pi->next) {
					if (pi->next == pitem) {
						pi->next = pitem->next;
						if (pqueue->tail == pitem)
							pqueue->tail = pi;
						pitem->next = 0;
						break;
					}
				}
			}
		}
		gfxSystemUnlock();
	}
	bool_t gfxQueueASyncIsEmpty(gfxQueueASync *pqueue) {
		return pqueue->head == NULL;
	}
	bool_t gfxQueueASyncIsIn(gfxQueueASync *pqueue, gfxQueueASyncItem *pitem) {
		gfxQueueASyncItem *pi;

		gfxSystemLock();
		for(pi = pqueue->head; pi; pi = pi->next) {
			if (pi == pitem) {
				gfxSystemUnlock();
				return TRUE;
			}
		}
		gfxSystemUnlock();
		return FALSE;
	}
#endif

#if GQUEUE_NEED_GSYNC
	void gfxQueueGSyncInit(gfxQueueGSync *pqueue) {
		pqueue->head = pqueue->tail = 0;
		gfxSemInit(&pqueue->sem, 0, MAX_SEMAPHORE_COUNT);
	}
	gfxQueueGSyncItem *gfxQueueGSyncGet(gfxQueueGSync *pqueue, delaytime_t ms) {
		gfxQueueGSyncItem	*pi;

		if (!gfxSemWait(&pqueue->sem, ms)) return 0;
		gfxSystemLock();
		pi = pqueue->head;
		pqueue->head = pi->next;
		pi->next = 0;
		gfxSytemUnlock();
		return pi;
	}
	void gfxQueueGSyncPut(gfxQueueGSync *pqueue, gfxQueueGSyncItem *pitem) {
		pitem->next = 0;

		gfxSystemLock();
		if (!pqueue->head) {
			pqueue->head = pqueue->tail = pitem;
		} else {
			pqueue->tail->next = pitem;
			pqueue->tail = pitem;
		}
		gfxSystemUnlock();

		gfxSemSignal(&pqueue->sem);
	}
	void gfxQueueGSyncPush(gfxQueueGSync *pqueue, gfxQueueGSyncItem *pitem) {
		gfxSystemLock();
		pitem->next = pqueue->head;
		pqueue->head = pitem;
		if (!pitem->next)
			pqueue->tail = pitem;
		gfxSystemUnlock();

		gfxSemSignal(&pqueue->sem);
	}
	void gfxQueueGSyncRemove(gfxQueueGSync *pqueue, gfxQueueGSyncItem *pitem) {
		gfxQueueGSyncItem *pi;

		if (!pitem) return;
		gfxSystemLock();
		if (pqueue->head) {
			if (pqueue->head == pitem) {
				pqueue->head = pitem->next;
				pitem->next = 0;
			} else {
				for(pi = pqueue->head; pi->next; pi = pi->next) {
					if (pi->next == pitem) {
						pi->next = pitem->next;
						if (pqueue->tail == pitem)
							pqueue->tail = pi;
						pitem->next = 0;
						break;
					}
				}
			}
		}
		gfxSystemUnlock();
	}
	bool_t gfxQueueGSyncIsEmpty(gfxQueueGSync *pqueue) {
		return pqueue->head == NULL;
	}
	bool_t gfxQueueGSyncIsIn(gfxQueueGSync *pqueue, gfxQueueGSyncItem *pitem) {
		gfxQueueGSyncItem *pi;

		gfxSystemLock();
		for(pi = pqueue->head; pi; pi = pi->next) {
			if (pi == pitem) {
				gfxSystemUnlock();
				return TRUE;
			}
		}
		gfxSystemUnlock();
		return FALSE;
	}
#endif

#if GQUEUE_NEED_FSYNC
	void gfxQueueFSyncInit(gfxQueueFSync *pqueue) {
		pqueue->head = pqueue->tail = 0;
		gfxSemInit(&pqueue->sem, 0, MAX_SEMAPHORE_COUNT);
	}
	gfxQueueFSyncItem *gfxQueueFSyncGet(gfxQueueFSync *pqueue, delaytime_t ms) {
		gfxQueueFSyncItem	*pi;

		if (!gfxSemWait(&pqueue->sem, ms)) return 0;
		gfxSystemLock();
		pi = pqueue->head;
		pqueue->head = pi->next;
		pi->next = 0;
		gfxSytemUnlock();

		gfxSemSignalI(&pi->sem);
		gfxSemDestroy(&pi->sem);
		return pi;
	}
	bool_t gfxQueueFSyncPut(gfxQueueFSync *pqueue, gfxQueueFSyncItem *pitem, delaytime_t ms) {
		gfxSemInit(&pitem->sem, 0, 1);
		pitem->next = 0;

		gfxSystemLock();
		if (!pqueue->head) {
			pqueue->head = pqueue->tail = pitem;
		} else {
			pqueue->tail->next = pitem;
			pqueue->tail = pitem;
		}
		gfxSystemUnlock();

		gfxSemSignal(&pqueue->sem);
		return gfxSemWait(&pitem->sem, ms);
	}
	bool_t gfxQueueFSyncPush(gfxQueueFSync *pqueue, gfxQueueFSyncItem *pitem, delaytime_t ms) {
		gfxSemInit(&pitem->sem, 0, 1);

		gfxSystemLock();
		pitem->next = pqueue->head;
		pqueue->head = pitem;
		if (!pitem->next)
			pqueue->tail = pitem;
		gfxSystemUnlock();

		gfxSemSignal(&pqueue->sem);
		return gfxSemWait(&pitem->sem, ms);
	}
	void gfxQueueFSyncRemove(gfxQueueFSync *pqueue, gfxQueueFSyncItem *pitem) {
		gfxQueueFSyncItem *pi;

		if (!pitem) return;
		gfxSystemLock();
		if (pqueue->head) {
			if (pqueue->head == pitem) {
				pqueue->head = pitem->next;
			found:
				pitem->next = 0;
				gfxSystemUnlock();
				gfxSemSignal(&pitem->sem);
				gfxSemDestroy(&pitem->sem);
				return;
			}
			for(pi = pqueue->head; pi->next; pi = pi->next) {
				if (pi->next == pitem) {
					pi->next = pitem->next;
					if (pqueue->tail == pitem)
						pqueue->tail = pi;
					goto found;
				}
			}
		}
		gfxSystemUnlock();
	}
	bool_t gfxQueueFSyncIsEmpty(gfxQueueFSync *pqueue) {
		return pqueue->head == NULL;
	}
	bool_t gfxQueueFSyncIsIn(gfxQueueFSync *pqueue, gfxQueueFSyncItem *pitem) {
		gfxQueueASyncItem *pi;

		gfxSystemLock();
		for(pi = pqueue->head; pi; pi = pi->next) {
			if (pi == pitem) {
				gfxSystemUnlock();
				return TRUE;
			}
		}
		gfxSystemUnlock();
		return FALSE;
	}
#endif

#endif /* GFX_USE_GQUEUE */
