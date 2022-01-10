//
// Laser Angry Lasers
// By Vander R. N. Dias a.k.a. "imerso" @ imersiva.com
// Copyright(c) 2019 Wicked Lasers
//

#ifndef _TNYSPLINE_H_
#define _TNYSPLINE_H_

#include <memory>
	#include <vector>
    #include "ldCore/Helpers/Maths/ldMaths.h"

		class CSPLine
		{

			public:

				CSPLine();
                virtual ~CSPLine() {}

                void				SetContinuity(float fContinuity);
                float				Continuity() { return m_fContinuity; }

                void				SetBias(float fBias);
                float				Bias() { return m_fBias; }

                void				SetTension(float fTension);
                float				Tension() { return m_fTension; }

                void				AddKey(std::unique_ptr<ldVec2> pVector);
                inline int			KeyCount() { return (int)m_Keys.size(); }
                ldVec2				Point(float fFrame);

			private:

                std::vector<std::unique_ptr<ldVec2>>	m_Keys;           // key list

                float				m_fContinuity;    // Continuity (-1.0f .. 1.0f)
                float				m_fBias;          // Bias (-1.0f .. 1.0f)
                float				m_fTension;       // Tension (-1.0f .. 1.0f)

		};

#endif
