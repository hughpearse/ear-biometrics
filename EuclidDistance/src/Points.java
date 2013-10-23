import weka.core.Attribute;
import weka.core.FastVector;
import weka.core.Instance;
import weka.core.Instances;


public class Points {

	/**
	 * Create an Instance of the same type as the Instances object you are searching in.
	 * @param p - a 3D point
	 * @param dataset - the dataset you are searching in, which was used to build the KDTree
	 * @return an Instance that the nearest neighbor can be found for
	 */
	public Instance createInstance(Integer points[], final Instances dataset){
	    // Create numeric attributes "x" and "y" and "z"
		Attribute attr[] = new Attribute[10];
		for(int i=0; i<points.length; i++){
			attr[i] = new Attribute("p" + i);
		    attr[i] = dataset.attribute(i);
		}
	 
	    // Create vector of the above attributes
	    FastVector attributes = new FastVector(points.length);
	    for(int i=0; i<points.length; i++)
	    	attributes.addElement(attr[i]);
	 
	    // Create empty instance with X attribute values
	    Instance inst = new Instance(points.length);               
	     
	    // Set instance's values for the attributes "x", "y", and "z"
	    for(int i=0; i<points.length; i++)
	    	inst.setValue(attr[i], points[i]);
	 
	    // Set instance's dataset to be the dataset "points1"
	    inst.setDataset(dataset);
	                 
	    return inst;   
	}

}
