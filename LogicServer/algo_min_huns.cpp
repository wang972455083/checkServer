

#include <memory.h>
#include <algorithm>
#include "algo_min_huns.h"


// 0表示允许混做已经四张以上的牌
#define TILE_BLOCK_MIN_HUNS__CHECK_AVAILABLE_TILES 0

// 字牌只能成刻子，所以很简单
static unsigned tile_block_min_huns__zipai(unsigned const p[9], unsigned head, unsigned tail) {
	unsigned min_hun = 0;
	for (unsigned i = head; i <= tail; ++i) {
		// 4张同样的字牌，不能组顺子，组了一个刻子，还剩一张，万能牌也不能再去取这个值，所以肯定胡不了
		if (TILE_BLOCK_MIN_HUNS__CHECK_AVAILABLE_TILES && p[i] == 4) return 999;
		min_hun += (3 - (p[i] % 3)) % 3;
	}
	return min_hun;
}

// 经过测试，研究了一些例子，发现还是得递归去解，取最小值
static unsigned tile_block_min_huns__shupai(unsigned p[9], unsigned head, unsigned tail, unsigned jiang_pos) {
	unsigned min_hun = 0;
	// 从起始端开始消
	for (unsigned i = head; i <= tail; ++i) {
		// 先把这个位置上的所有的刻子消掉
		// 可以反证为什么这么做：假设这些刻子能节省万能牌，那它们肯定是和后面的牌组顺子用，那有分三种情况：
		//   能组1个顺子：组不组都需要一张万能牌，不组的话，后面这两张牌还可能跟再往后的组顺子用
		//   能组2个顺子：组的话，这个刻子就缺了，也得补万能牌啊，不组的话，后面的还可能跟更后面的组顺子用
		//   能组3个顺子：那也是能组3个刻子啊，都不需要补充万能牌
		p[i] = p[i] % 3;
		// 然后这个p[i]就有0、1、2这三种情况
		if (0 == p[i]) continue; // 这个位置空了，没事了
		unsigned has1 = i + 1 <= tail ? p[i + 1] : 0;
		unsigned has2 = i + 2 <= tail ? p[i + 2] : 0;
		if (1 == p[i]) {
			// 还剩一张牌，那最好去跟后面组顺子，不然得给它补两张万能牌呢
			// 彼此之间都是近距离的，除非在末端后面没牌了，不然最多补一张万能牌肯定能组出顺子
			if (has1 && has2) {
				// 能组顺子，那就消掉吧
				--p[i + 1];
				--p[i + 2];
			} else if (has1 || has2) {
				// 补一张万能牌就能组顺子了
				if (has1) --p[i + 1];
				else --p[i + 2];
				min_hun += 1;
			} else {
				// 不然得浪费两张万能牌了
				p[i] = 0;
				min_hun += 2;
			}
		} else {
			// 还剩两张牌，补1张万能牌能成刻子，也可以和后面组成2个顺子，也可以和后面组成1个顺子
			// 这三种解必须递归算，取最小值
			// 先看2个顺子这种分支的，有机会提前跳出的
			if (has1 >= 2 && has2 >= 2) {
				unsigned tmp[9];
				memcpy(tmp, p, sizeof(tmp));
				tmp[i] = 0;
				tmp[i + 1] -= 2;
				tmp[i + 2] -= 2;
				unsigned needed_hun__shunzi2_branch = tile_block_min_huns__shupai(tmp, i + 1, tail, jiang_pos);
				if (needed_hun__shunzi2_branch <= 1) {
					min_hun += needed_hun__shunzi2_branch;
					return min_hun;
				}
				// 自己补1张万能牌成刻子吧
				p[i] = 0;
				unsigned needed_hun__kezi_brance;
				// 在将的位置就不能用万能牌补刻子了
				if (TILE_BLOCK_MIN_HUNS__CHECK_AVAILABLE_TILES && i == jiang_pos) needed_hun__kezi_brance = 999;
				else needed_hun__kezi_brance = 1 + tile_block_min_huns__shupai(p, i + 1, tail, jiang_pos);
				min_hun += std::min(needed_hun__kezi_brance, needed_hun__shunzi2_branch);
				return min_hun;
			} else if (has1 >= 1 && has2 >= 1) {
				unsigned tmp[9];
				memcpy(tmp, p, sizeof(tmp));
				tmp[i] -= 1;
				tmp[i + 1] -= 1;
				tmp[i + 2] -= 1;
				unsigned needed_hun__shunzi1_branch = tile_block_min_huns__shupai(tmp, i, tail, jiang_pos);
				if (needed_hun__shunzi1_branch <= 1) {
					min_hun += needed_hun__shunzi1_branch;
					return min_hun;
				}
				// 自己补1张万能牌成刻子吧
				p[i] = 0;
				unsigned needed_hun__kezi_brance;
				// 在将的位置就不能用万能牌补刻子了
				if (TILE_BLOCK_MIN_HUNS__CHECK_AVAILABLE_TILES && i == jiang_pos) needed_hun__kezi_brance = 999;
				else needed_hun__kezi_brance = 1 + tile_block_min_huns__shupai(p, i + 1, tail, jiang_pos);
				min_hun += std::min(needed_hun__kezi_brance, needed_hun__shunzi1_branch);
				return min_hun;
			} else {
				min_hun += 1;
				// 相当于尾递归
			}
		}
	}
	return min_hun;
}



// 数组元素表示这个位置上有几张重复的牌，比如1万到9万9个位置
unsigned malgo_min_huns(unsigned const p[9], bool is_zipai, unsigned jiang_pos) {
	// 字牌就不用分块去算了
	if (is_zipai) return tile_block_min_huns__zipai(p, 0, 8);
	// 数牌按距离分块是为了降低分支组合，由乘法关系变成加法关系
	unsigned min_hun = 0;
	// 拆分成block，block之间至少要有2个空位
	for (unsigned i = 0; i <= 8; ) {
		if (0 == p[i]) {
			++i;
		} else {
			unsigned j = i;
			unsigned block[9] = { 0 };
			// 找到block的末尾 [i, j]
			for (;;) {
				block[j] = p[j];
				if (8 == j) break;
				if (0 == p[j + 1]) {
					if (7 == j) break;
					if (0 == p[j + 2]) break;
				}
				++j;
			}
			min_hun += tile_block_min_huns__shupai(block, i, j, jiang_pos);
			i = j + 3;
		}
	}
	return min_hun;
}

