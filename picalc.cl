__kernel void calculate_pi (int operands_per_item, __local float* local_result, __global float* global_result) {

	// get local and global id and sizes
	int lid = get_local_id(0);
	int gid = get_global_id(0);
	int lsize = get_local_size();
	int gsize = get_global_size();

	// initialize local values to zero
	for(int i = 0; i < lsize; i++)
		local_result[i] = 0;

	// wait for local results to be initialized to 0	
	barrier(CLK_LOCAL_MEM_FENCE);

	// set work item index
	int index = 2 * gid + 1;

	for(int i = index; i < (gsize + 1) * 2; i += 2 * operands_per_item)
		if(gid % 2 == 0)
			
		
		
}
