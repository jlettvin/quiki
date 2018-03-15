#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <vector>
#include <tuple>
#include <map>

using namespace std;


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
