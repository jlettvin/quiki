#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <vector>
#include <tuple>
#include <map>

using namespace std;

/*
class Plane
{
	public:

		enum { ERROR, ACCEPT, REJECT };

		unsigned char uc(char c) const { return static_cast<unsigned char>(c); }

		//---------------------------------------------------------------------
		Plane ()
		//---------------------------------------------------------------------
		{
			memset(m_plane, ERROR, 256);
		}

		//---------------------------------------------------------------------
		Plane (const string& s_accept, const string& s_reject)
		//---------------------------------------------------------------------
		{
			memset(m_plane, ERROR, 256);
			accept (s_accept);
			reject (s_reject);
		}

		//---------------------------------------------------------------------
		Plane (size_t iA, size_t iZ)
		//---------------------------------------------------------------------
		{
			memset(m_plane, ERROR, 256);
			while (iA <= iZ) m_plane[uc(iA++)] = ACCEPT;
		}

		//---------------------------------------------------------------------
		Plane& error (const string&  s_error)
		//---------------------------------------------------------------------
		{
			for (auto c:  s_error) m_plane[uc(c)] = ERROR;
			return *this;
		}

		//---------------------------------------------------------------------
		Plane& accept (const string& s_accept)
		//---------------------------------------------------------------------
		{
			for (auto c: s_accept) m_plane[uc(c)] = ACCEPT;
			return *this;
		}

		//---------------------------------------------------------------------
		Plane& reject (const string& s_reject)
		//---------------------------------------------------------------------
		{
			for (auto c: s_reject) m_plane[uc(c)] = REJECT;
			return *this;
		}

		//---------------------------------------------------------------------
		Plane& operator ()(char a, char z)
		//---------------------------------------------------------------------
		{
			while (a <= z) m_plane[uc(a++)] = ACCEPT;
		}

		//---------------------------------------------------------------------
		Plane& operator ()(const string& s_accept)
		//---------------------------------------------------------------------
		{
			accept (s_accept);
		}

		//---------------------------------------------------------------------
		Plane& operator ()(const Plane& other)
		//---------------------------------------------------------------------
		{
			for (size_t ii=0; ii<256; ++ii)
			{
				unsigned char uc{other.m_plane[ii]};
				if (uc) m_plane[ii] = uc;
			}
		}

		//---------------------------------------------------------------------
		char operator ()(char c)
		//---------------------------------------------------------------------
		{
			return m_plane[c];
		}

		//---------------------------------------------------------------------
		string operator*()
		//---------------------------------------------------------------------
		{
		static const char* hex{"0123456789ABCDEF"};
			string sReturn;
			for (size_t ii=0; ii<256; ++ii)
			{
		if ((ii == 0) && m_plane[ii]) sReturn += "\\x00";
		else if ((ii < '0' || ii > '~') && m_plane[ii])
		{
			sReturn += "\\x";
			sReturn += hex[(ii>>4)&0xf];
			sReturn += hex[(ii   )&0xf];
		}
		else if (m_plane[ii]) sReturn += static_cast<char>(ii);
			}
			return sReturn;
		}
    
		//---------------------------------------------------------------------
    bool named (const string& sName)
		//---------------------------------------------------------------------
    {
      return (m_named.find (sName) != m_named.end ());
    }

		//---------------------------------------------------------------------
		Plane& operator[](const string& sName)
		//---------------------------------------------------------------------
		{
			map<string,Plane*>::iterator iter = m_named.find(sName);
			if (iter == m_named.end ())
			{
				cout << "\t\tINITIALIZING" << endl;
				m_named["every"] = new Plane(0, 255);
				m_named["upper"] = new Plane();
				m_named["lower"] = new Plane();
				m_named["alpha"] = new Plane();
				m_named["digit"] = new Plane();
				m_named["alnum"] = new Plane();
				m_named["token"] = new Plane();
				m_named["space"] = new Plane();
				m_named["white"] = new Plane();
				m_named["punct"] = new Plane();
				m_named["dark" ] = new Plane();
				m_named["newln"] = new Plane();

				(*m_named["digit"]) ('0', '9');
				(*m_named["upper"]) ('A', 'Z');
				(*m_named["lower"]) ('a', 'z');
				(*m_named["alpha"]) (*m_named["upper"]);
				(*m_named["alpha"]) (*m_named["lower"]);
				(*m_named["alnum"]) (*m_named["alpha"]);
				(*m_named["alnum"]) (*m_named["digit"]);
				(*m_named["token"]) (*m_named["alnum"]);
				(*m_named["token"]) ("_");
				(*m_named["punct"]) ("\"!#$%&'()*+,-./:;<=>?@[]^_`{|}~");
				(*m_named["space"]) (" \t\b");
				(*m_named["newln"]) ("\n\r");
				(*m_named["white"]) (*m_named["space"]);
				(*m_named["white"]) (*m_named["newln"]);
				(*m_named["dark"])  (*m_named["alnum"]);
				(*m_named["dark"])  (*m_named["punct"]);

				//(*m_named["every"]) (*m_named["dark" ]);
				//(*m_named["every"]) (*m_named["white"]);

				for (auto keyed: m_named)
				{
					Plane& plane{*keyed.second};
					cout << keyed.first << ": " << *plane << endl;
				}
			}
			return *m_named[sName];
		}

		//---------------------------------------------------------------------
		bool error  (char c) const { return (m_plane[uc(c)] ==  ERROR); }
		bool accept (char c) const { return (m_plane[uc(c)] == ACCEPT); }
		bool reject (char c) const { return (m_plane[uc(c)] == REJECT); }
		//---------------------------------------------------------------------

	private:

		unsigned char m_plane[256];
		static map<string,Plane*> m_named;
};
map<string,Plane*> Plane::m_named;

class Grammar
{
public:
		//---------------------------------------------------------------------
		Grammar (const char* sFilename)
			: m_self (*this)
			, m_sFilename (sFilename)
		//---------------------------------------------------------------------
		{
			ifstream ifs (m_sFilename);
			getline (ifs, m_sText, (char) ifs.eof ());
			ifs.close ();
			m_iOff = 0;
			m_iLimit = m_sText.size();
			cout << string (60, '1') << endl;
			parse ();
			cout << string (60, '2') << endl;
		}

		//---------------------------------------------------------------------
		~Grammar () { }
		//---------------------------------------------------------------------

		//---------------------------------------------------------------------
		string operator*()
		//---------------------------------------------------------------------
		{
			stringstream ss;
			return ss.str ();
		}

		//---------------------------------------------------------------------
		void ignore ()
		//---------------------------------------------------------------------
		{
		
			while (
					m_iOff < m_iLimit &&
					!m_see["newln"] (m_sText[m_iOff])
		  )
		{
			++m_iOff;
		}
		}

	//---------------------------------------------------------------------
	bool advance (const string& sStart, const string& sAllow)
	//---------------------------------------------------------------------
	{
		if ((m_iOff < m_iLimit) && m_see[sStart] (m_sText[m_iOff]))
		{
			++m_iOff;
			while ((m_iOff < m_iLimit) && m_see[sAllow] (m_sText[m_iOff]))
		{
			++m_iOff;
		}
		return true;
		}
		ignore ();
		return false;
	}

	//---------------------------------------------------------------------
	bool advance (char c)
	//---------------------------------------------------------------------
	{
		bool bFound = (m_sText[m_iOff] == c);
		m_iOff += bFound;
		if (!bFound) ignore ();
		return bFound;
	}

	//---------------------------------------------------------------------
	bool resembles (vector<string>& vsType, vector<size_t>& viOffset)
	//---------------------------------------------------------------------
	{
		for (auto& aType: vsType)
		{
			space ();
			viOffset.emplace_back (m_iOff);
			if (m_see.named (aType))
			{
				if (!advance (aType, aType)) return false;
			}
			else
			{
				if (!exactly (aType)) return false;
			}
			viOffset.emplace_back (m_iOff);
		}
		return true;
	}

	//---------------------------------------------------------------------
	bool exactly (const string& match)
	//---------------------------------------------------------------------
	{
		space ();
		size_t jj = m_iOff;
		for (size_t I=match.size (), ii=0; ii<I; ++ii, ++jj)
		{
			if ((jj >= m_iLimit) || (match[ii] != m_sText[jj])) return false;
		}
		m_iOff = jj;
		return true;
	}

	//---------------------------------------------------------------------
	void advanceUntil (const string& sStop)
	//---------------------------------------------------------------------
	{
		while ((m_iOff < m_iLimit) && !m_see[sStop] (m_sText[m_iOff])) m_iOff++;
	}

	//---------------------------------------------------------------------
	bool token (size_t& iStart, size_t& iEnd)
	//---------------------------------------------------------------------
	{
		space ();
		iEnd = iStart = m_iOff;
		if (advance ("alpha", "token"))
		{
			iEnd = m_iOff;
			return true;
		}
		return false;
	}

	//---------------------------------------------------------------------
	bool rule (vector<string>& vsRule, string& sFunction)
	//---------------------------------------------------------------------
	{
		bool bContains = false;
		bool bQuoted   = false;
		bool bFunction = false;
		size_t iA, iB;
		for (;;)
		{
			space();
			if (m_iOff >= m_iLimit || m_see["newln"] (m_sText[m_iOff]))
			{
				if (bQuoted) return false;
			}
			char c = m_sText[m_iOff];
			switch (c)
			{
				case '"':
					bQuoted = !bQuoted;
					continue;
					break;
				case '[':
					bFunction = true;
			}
			iA = m_iOff;
			if (bQuoted)
			{
			}
			else
			{
				if (!advance("alpha", "token")) return false;
				vsRule.emplace_back (m_sText.substr (iA, m_iOff - iA));
			}
		}
		return bContains;
	}

	//---------------------------------------------------------------------
	bool space ()
	//---------------------------------------------------------------------
	{
		if (advance ("space", "space"))
		{
			return true;
		}
		return false;
	}

	//---------------------------------------------------------------------
	void parse ()
	//---------------------------------------------------------------------
	{
		string& buf{m_sText};
		for (m_iOff = 0; m_iOff < m_iLimit; ++m_iOff)
		{
			// Pass leading space;
			size_t iA, iB, iC, iD, iE, iF;
			vector<string> vsLabel{"[", "label", "=", "\""};
			vector<size_t> viOffset{};

			if (!token (iA, iB)) continue;
			if (!exactly ("->")) continue;
			if (!token (iC, iD)) continue;
			if (!resembles (vsLabel, viOffset)) continue;
			iE = m_iOff;
			advanceUntil ("newln");
			iF = m_iOff;
			// Show off
			cout
				<< buf.substr(iA, iB-iA)
				<< " -> "
				<< buf.substr(iC, iD-iC)
				<< buf.substr(iE, iF-iE)
				<< endl;
		}
	}

private:

		typedef tuple<string, string, string, string, string> rule_t;

		Grammar&		m_self;
		string		  m_sFilename;
		string		  m_sText;
		size_t		  m_iOff;
		size_t		  m_iLimit;
		vector<rule_t>  m_rule;
		Plane		   m_see;
};
*/

/** Dot parses simple non-hierarchical graphviz digraph edges into 4tuples.
 * <0>  start   state
 * <1>  stop	state
 * <2>  function
 * <3>  lexical tokens
 * <4>  rule	number
 */
class Dot
{
	public:
		//---------------------------------------------------------------------
		Dot (const char* sFilename)
			: m_sFilename (sFilename)
			, m_self (*this)
		//---------------------------------------------------------------------
		{
			ifstream ifs (m_sFilename);
			getline (ifs, m_sContents, (char) ifs.eof ());
			ifs.close ();
			uncomment ().reline ().rules ();
		}

		//---------------------------------------------------------------------
		~Dot ()
		//---------------------------------------------------------------------
		{
			for (auto& s: m_vsRule)
			{
				cout
					<< get<0>(s) << ":" 
					<< get<1>(s) << ":" 
					<< get<2>(s) << ":"
					<< get<3>(s) << ":"
					<< get<4>(s) << endl;
			}
		}

	private:

		typedef tuple<string, string, string, string, string> rule_t;

		string m_sFilename;
		string m_sContents;
		vector<string> m_vsLine;
		vector<rule_t> m_vsRule;
		Dot& m_self;
		const char* m_sSpace{" \t"};

		//---------------------------------------------------------------------
		string trim (const string& str)
		//---------------------------------------------------------------------
		{
			size_t first = str.find_first_not_of(m_sSpace);
			if (first == string::npos)
				return "";
			size_t last = str.find_last_not_of(m_sSpace);
			return str.substr(first, (last-first+1));
		}

		//---------------------------------------------------------------------
		string escReplace (const string& s, const string& from, const string& to)
		//---------------------------------------------------------------------
		{
			string result = s;
			auto pos = s.find (from);
			if (pos == string::npos)
			{
				return result;
			}
			result.replace (pos, from.length (), to);
			return escReplace (result, from, to);
		} // escReplace



		//---------------------------------------------------------------------
		string unescape (const string& s)
		//---------------------------------------------------------------------
		{
			static vector< pair< string, string > > patterns = {
				{ "\\\\" , "\\" }
				,{ "\\n", "\n" }
				,{ "\\r", "\r" }
				,{ "\\t", "\t" }
				,{ "\\\"", "\"" }
			};
			string result = s;
			for (const auto & p : patterns)
			{
				result = escReplace (result, p.first, p.second);
			}
			return result;
		} // unescape

		//---------------------------------------------------------------------
		string doescape (const string& s)
		//---------------------------------------------------------------------
		{
			stringstream ss{s};
			string result;
			while (!ss.eof ())
			{
				string token;
				ss >> token;
				size_t iSize = token.size();
				if (token[0] == '"' && token[iSize-1] == '"')
				{
					token = token.substr (1, iSize-2);
					result += " \"";
					for (auto c: token)
					{
						if (c == '"')
						{
							result += "\\\"";
						}
						else
						{
							result += c;
						}
					}
					result += "\"";
				}
				else
				{
					result += " " + token;
				}
			}

			return result;
		}


		//---------------------------------------------------------------------
		Dot& wipe (string sA, string sZ, bool keep=false)
		//---------------------------------------------------------------------
		{
			stringstream ss;
			size_t c1 = 0, c0 = m_sContents.find (sA);
			size_t iA = sA.size ();
			size_t iZ = sZ.size ();
			while (c0 != string::npos)
			{
				ss << m_sContents.substr (c1, c0 - c1);
				c0 += iA;
				c1 = m_sContents.find (sZ, c0 + iA);
				if (c1 == string::npos)
				{
					cout << "Unterminated comment" << endl;
					exit (1);
				}
				if (!keep) c1 += iZ;
				c0 = m_sContents.find (sA, c1);
			}
			if (c1 != string::npos)
			{
				ss << m_sContents.substr (c1);
			}
			m_sContents = ss.str ();
		}

		//---------------------------------------------------------------------
		Dot& uncomment ()
		//---------------------------------------------------------------------
		{
			wipe ("/*", "*/");
			wipe ("//", "\n", true);
			return m_self;
		}

		//---------------------------------------------------------------------
		Dot& reline()
		//---------------------------------------------------------------------
		{
			char sep = '\n';
			string::size_type b = 0;

			while ((b = m_sContents.find_first_not_of (sep, b)) != string::npos) {
				auto e = m_sContents.find_first_of (sep, b);
				m_vsLine.push_back (m_sContents.substr (b, e-b));
				b = e;
			}
			return m_self;
		}

		//---------------------------------------------------------------------
		Dot& rules ()
		//---------------------------------------------------------------------
		{
			// S1_root -> S1_root[label="0. dark  - \"<\""];
			// S1_root -> S2_oTag[label="2. \"<\""];
			for (auto s: m_vsLine)
			{
				// Only lines having ->
				size_t iArrow = s.find ("->");
				if (iArrow == string::npos) continue;
				//cout << "\t\t1 " << s << endl;
				string sSource = trim (s.substr (0, iArrow));

				// Only lines with '[' and ']'
				size_t iOpen = s.find_first_of ('[', iArrow + 1);
				if (iOpen == string::npos) continue;
				//cout << "\t\t2 " << s << endl;
				string sTarget = trim (s.substr (iArrow + 2, iOpen - iArrow - 2));
				iOpen++;
				size_t iClose = s.find_last_of (']');
				if (iClose == string::npos) continue;
				iClose--;

				// Only lines with "label" followed by '='
				string sRule = trim (s.substr (iOpen, iClose - iOpen + 1));
				if (sRule.compare(0, 5, "label")) continue;
				//cout << "\t\t3 " << sRule << endl;
				sRule = trim (sRule.substr (5));
				if (sRule[0] != '=') continue;
				sRule = trim (sRule.substr (1));

				// Only lines with a label string
				if (sRule[0] != '"' || sRule[sRule.size()-1] != '"') continue;
				sRule = sRule.substr (1, sRule.size() - 2);
				cout << "\t\t4 " << sRule << endl;

				// Check for function name
				string sFunction = "";
				size_t iEnd = sRule.size();
				//cout << "\t\t5 " << sRule << endl;
				if (sRule[iEnd-1] == ']')
				{
					size_t iStart = sRule.find_last_of ('[');
					if (iStart != string::npos)
					{
						sFunction = trim (sRule.substr (iStart+1, iEnd-iStart-2));
						sRule = trim (sRule.substr (0, iStart-1));
						//cout << "\t\t6 " << sRule << "$" << sFunction << endl;
					}
					else
					{
						cout << "unclosed function" << endl;
						exit (1);
					}
				}

				//cout << "\t\t6 " << sRule << endl;
				// Strip numeric label
				string sNumber = "";
				char c = sRule[0];
				if (c >= '0' && c <= '9')
				{
					size_t iDot = sRule.find_first_not_of ("0123456789");
					if (iDot != string::npos && sRule[iDot] == '.')
					{
						sNumber = sRule.substr (0, iDot);
						sRule = trim (sRule.substr (iDot+1));
					}
				}

				// Convert to C string
				sRule = unescape (sRule);
				sRule = trim (doescape (sRule));

				m_vsRule.emplace_back (
						sNumber,
						sSource,
						sTarget,
						sFunction,
						sRule);
			}
			return m_self;
		}

};

int main (int argc, char** argv)
{
	while (--argc)
	{
		const char* sFilename = *++argv;
		{ Dot	 dot (sFilename);}
		//{ Grammar grm (sFilename);}
	}
	return 0;
}
