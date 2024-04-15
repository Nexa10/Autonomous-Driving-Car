const fetchData = async () => {
    try {
        const response = await fetch('');
      
    
        console.log(response)
        
    } catch (error) {
        console.error('Failed to fetch data:', error);
    }
};

// Call fetchData once when the component mounts
fetchData();