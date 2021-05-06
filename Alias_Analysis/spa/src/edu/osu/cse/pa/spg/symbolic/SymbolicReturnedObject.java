package edu.osu.cse.pa.spg.symbolic;

import java.util.Iterator;

import soot.SootMethod;
import soot.Type;


public class SymbolicReturnedObject extends SymbolicObject {

	public SymbolicReturnedObject(SootMethod m, Type basicType) {
		super(basicType, m);

	}


	public String toString() {
		return "[SRO] type: "+ basicType+ " for " + method.getSignature()+" hash@"+ Integer.toHexString(hashCode()) ;
	}
}
