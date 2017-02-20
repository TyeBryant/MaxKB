#ifndef KF_RANDOM_HEADER
#define KF_RANDOM_HEADER

#include "kf/kf_types.h"
#include "kf/kf_math.h"
#include "kf/kf_vector.h"
// hashRJ32 - http://www.concentric.net/~Ttwang/tech/inthash.htm
// Xor128 - http://en.wikipedia.org/wiki/Xorshift

namespace kf
{

	inline kf::u32 hashRJ32( kf::u32 a)
	{
		a = (a+0x7ed55d16) + (a<<12);
		a = (a^0xc761c23c) ^ (a>>19);
		a = (a+0x165667b1) + (a<<5);
		a = (a+0xd3a2646c) ^ (a<<9);
		a = (a+0xfd7046c5) + (a<<3);
		a = (a^0xb55a4f09) ^ (a>>16);
		return a;
	}

	class Xor128
	{
	public:
		Xor128():m_x(123456789),m_y(362436069),m_z(521288629),m_w(88675123)
		{
		}
		kf::u32 operator()()
		{
			kf::u32 t;
			t = m_x ^ (m_x << 11);
			m_x = m_y; m_y = m_z; m_z = m_w;
			return m_w = m_w ^ (m_w >> 19) ^ (t ^ (t >> 8));
		}
		void operator()(kf::u32 seed)
		{
			 m_x = hashRJ32(seed);
			 m_y = hashRJ32(m_x);
			 m_z = hashRJ32(m_y);
			 m_w = hashRJ32(m_z);
		}
		double norm()
		{
			return double((*this)())/double(0xffffffff);
		}
	protected:
		kf::u32 m_x;
		kf::u32 m_y;
		kf::u32 m_z;
		kf::u32 m_w;
	};


}

#endif
