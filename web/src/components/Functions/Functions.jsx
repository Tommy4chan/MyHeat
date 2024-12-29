import FunctionCard from './FunctionCard';
import ColumnBlock from '@/components/layout/ColumnBlock';
import useFunctionStore from '@/store/functionStore';

const Functions = ({ isTwoCols = false }) => {
  const { functions } = useFunctionStore();

  return (
    <ColumnBlock>
      <h2 className="font-semibold text-2xl">Функції</h2>
      <div className={isTwoCols ? "grid grid-cols-1 lg:grid-cols-2 gap-4" : "flex flex-col gap-4"}>
        {functions?.map((customFunction, index) => (
          <FunctionCard 
            key={index}
            functionData={customFunction}
            functionIndex={index}
          />
        ))}
      </div>
    </ColumnBlock>
  );
};

export default Functions;