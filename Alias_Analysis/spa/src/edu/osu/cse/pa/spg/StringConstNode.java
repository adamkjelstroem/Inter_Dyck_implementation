package edu.osu.cse.pa.spg;

import soot.Scene;
import soot.SootMethod;
import soot.Type;
import soot.jimple.StringConstant;

import java.lang.StringBuilder;

public class StringConstNode extends AbstractAllocNode {

	// public static StringConstNode node = new StringConstNode();
	private StringConstant literal;

	public StringConstNode(SootMethod m, StringConstant literal) {
		super(m);
		this.literal = literal;
	}

	public StringConstant getStringLiteral() {
		return literal;
	}

	public Type getType() {
		return Scene.v().getRefType("java.lang.String");
	}

	public String escapeDelim(String s){
		int count=0;
		StringBuilder sb = new StringBuilder(s.length());
		for(int i=0;i<s.length();i++){
			char c = s.charAt(i);
			if(count==0 && c=='|'){
				count=1;
			}else if(count==1 && c=='|'){
				sb.append("\\");
			}else{
				count=0;
			}
			sb.append(c);
		}
		return sb.toString();
	}

	public String toString() {
		return "[strConst]" + escapeDelim(literal.toString()) + " in " + getMethod()+"hash@"+Integer.toHexString(hashCode());
	}

}
